'use strict';
// Koa-modules
const Koa = require('koa');
const router = require('koa-router')();
const bodyParser = require('koa-bodyparser');
const koaBody = require('koa-body');
const views = require('koa-views');
const statics = require('koa-static');
// Other modules
const path = require('path');
const ws = require('nodejs-websocket');
const rawBody = require('raw-body');
const fs = require('fs');
const session = require('./controller/sessionControl');
const crypto = require('crypto');
const WebSocketServer = require('websocket').server;
const http = require('http');
// Self-Design modules
const control = require('./controller/control');
const gamePlay = require('./controller/GamePlay');
const model = require('./model/model');
const gameDB = require('./model/gamePlayDB');



const app = new Koa();
app.use(session.session);

app.use(koaBody({
    formLimit: '100mb',
    multipart: true
}));
app.use(bodyParser());

/*
* POST Methods
*/
router.post('/login', async(ctx, next) => {
    var data = ctx.request.body;
    var username = data.name, password = data.pass;
    var loginRes = await control.Login(username, password);
    if (loginRes.code === 1) {
        ctx.session = {user: username, pass: password};
    }
    ctx.response.body = JSON.stringify(loginRes);
});

router.post('/logup', async(ctx, next) => {
    var data = ctx.request.query, req = ctx.req;
    var username = data.name, password = data.pass,
        imgType = data.imgType, rePass = data.rePass,
        phone = data.phone, email = data.email, headDefault = data.default;
    var image = null;
    if(headDefault == 0){
        image = await rawBody(ctx.req);
    }else {
        image = fs.readFileSync('static/img/user.jpg');
    }
    var logupRes = await control.Logup(username, password, rePass, image, imgType, phone, email);
    if (logupRes.code === 1) {
        ctx.session = {user: username, pass: password};
    }
    ctx.response.body = JSON.stringify(logupRes);
});

router.post('/userDeckInfo', async (ctx, next) => {
    var data = ctx.request.query;
    let requestedUser = data.username;
    if (typeof requestedUser === 'undefined') {
        await next();
        return;
    }
    let userDeck = await gameDB.QueryUserDecks(requestedUser);
    ctx.response.body = JSON.stringify({deckCount: userDeck.length});
});

router.get('/gameLogOut', async (ctx, next) => {
    let data = ctx.request.query;
    let name = data.username;
    let pass = data.password;
    if (!connections[getStringMD5(name)]) {
        ctx.response.body = "User has not signed in yet!";
        return;
    }
    if (await model.TestLogIn(name, pass) === 1) {
        connections[getStringMD5(name)].close();
        delete connections[getStringMD5(name)];
        ctx.response.body = "websocket connection deleted!";
    }
});

app.use(router.routes());

app.listen(17777);
console.log('Schwifty Server running on port 17777....');


/* ------  WebSocket Connection Server  ------ */

var server = http.createServer(function(request, response) {
    console.log((new Date()) + ' Received request for ' + request.url);
    response.writeHead(404);
    response.end();
});
server.listen(17700, function() {
    console.log((new Date()) + ' Server is listening on port 17700');
});

let wsServer = new WebSocketServer({
    httpServer: server,
    autoAcceptConnections: false
});

var connections = {};
let gameSessionArr = {};
let gameRequest = {};
let userLocations = {};

wsServer.on('request', function(request){
    console.log(new Date() + ' Connection accepted.');
    let conn = request.accept();

    conn.on('message', async function(data){
        let str = data.utf8Data;
        console.log(str);
        let instructions = str.split('&&');
        console.log(instructions);
        switch (instructions[0].toLowerCase()) {
            //login&&username&&password
            case 'login':
                if (instructions.length !== 3) {
                    conn.sendUTF("Invalid number of params");
                    return;
                }
                let code = await model.TestLogIn(instructions[1], instructions[2]);
                if (code === 1) {
                    if (connections[getStringMD5(instructions[1])]) {
                        conn.sendUTF('err||Already signed in!');
                        return;
                    }
                    conn.user = instructions[1];
                    var connectionID = getStringMD5(instructions[1]);
                    conn.id = connectionID;
                    conn.sendUTF(connectionID);
                    connections[connectionID] = conn;
                    console.log('Created ws: ' + connectionID);
                }
                break;
            //gameReq&&key&&targetUsername
            case 'gamereq':
                if (conn.id !== instructions[1]) {
                    conn.sendUTF('err||Invalid Request sender!');
                    return;
                }
                if (!connections[getStringMD5(instructions[2])]) {
                    conn.sendUTF('err||Requested User is not online!');
                    return;
                }
                let userDeck = await gameDB.QueryUserDecks(instructions[2]);
                let deckCount = userDeck.length;
                connections[getStringMD5(instructions[2])].sendUTF(`recReq||${connections[instructions[1]].user}||${deckCount}`);
                conn.sendUTF('reqPend||Request Sent');
                if (gameRequest[instructions[1]])
                    gameRequest[instructions[1]].push(getStringMD5(instructions[2]));
                else
                    gameRequest[instructions[1]] = [getStringMD5(instructions[2])];
                break;
            //accReq&&key&&senderUsername
            case 'accreq':
                let targetUserMD5 = getStringMD5(instructions[2]);
                if (conn.id !== instructions[1]) {
                    conn.sendUTF('err||Invalid Request User!');
                    return;
                }
                if (!connections[getStringMD5(instructions[2])]) {
                    conn.sendUTF('err||Sender User is not online!');
                    return;
                }
                if (!gameRequest[targetUserMD5] ||
                    gameRequest[targetUserMD5].indexOf(instructions[1]) === -1) {
                    conn.sendUTF('err||No relevant request found!');
                    return;
                }
                let sessionKey = Math.random().toString(36).substring(3);
                gameRequest[targetUserMD5].splice(gameRequest[targetUserMD5].indexOf(instructions[1]), 1);
                conn.sendUTF(`reqAc||${instructions[2]}||${sessionKey}`);
                connections[targetUserMD5].sendUTF(`reqAc||${connections[instructions[1]].user}||${sessionKey}`);
                gameSessionArr[sessionKey] = new gamePlay.session(connections[instructions[1]].user, instructions[2]);
                break;
            //allReq&&key
            case 'allreq':
                let returnMsg = 'allReq';
                if (conn.id !== instructions[1]) {
                    conn.sendUTF('err||Invalid Request User!');
                    return;
                }
                for (let sender in gameRequest) {
                    if (gameRequest[sender].indexOf(instructions[1]) !== -1) {
                        returnMsg += "||";
                        returnMsg += connections[sender].user + "||";
                        let userDeck = await gameDB.QueryUserDecks(connections[sender].user);
                        let deckCount = userDeck.length;
                        returnMsg += deckCount;
                    }
                }
                conn.sendUTF(returnMsg);
                break;
             //getDeck&&sessionKey
            case 'getdeck':
                if (!gameSessionArr[instructions[1]]) {
                    conn.sendUTF('err||Requested session invalid!');
                    return;
                }
                if (gameSessionArr[instructions[1]].UserOne === conn.user) {
                    let userDeck = gameSessionArr[instructions[1]].deckOne;
                    let returnMsg = "deck";
                    for (let card of userDeck) {
                        returnMsg += "||" + card;
                    }
                    conn.sendUTF(returnMsg);
                }
                else if (gameSessionArr[instructions[1]].UserTwo === conn.user) {
                    let userDeck = gameSessionArr[instructions[1]].deckTwo;
                    let returnMsg = "deck";
                    for (let card of userDeck) {
                        returnMsg += "||" + card;
                    }
                    conn.sendUTF(returnMsg);
                }
                else conn.sendUTF('err||User is not in the session!');
                break;
            //getAllDeck&&username
            case 'getalldeck':
                if (!!instructions[1]) {
                    try {
                        let allDeck = await gameDB.QueryUserDecks(instructions[1]);
                        let msg = `allDeck||${instructions[1]}`;
                        for (let card of allDeck) {
                            msg += "||" + card;
                        }
                        conn.sendUTF(msg);
                    }
                    catch (err) {
                        conn.sendUTF('err||Failed to get user deck.');
                    }
                }
                break;
            //play&&sessionKey&&index
            case 'play':
                if (!gameSessionArr[instructions[1]]) {
                    conn.sendUTF('err||Requested session invalid!');
                    return;
                }
                else {
                    let result = gameSessionArr[instructions[1]].playCard(conn.user, instructions[2]);
                    if (result.status) {
                        conn.sendUTF('play||Success');
                        if (conn.user === gameSessionArr[instructions[1]].UserOne) {
                            connections[getStringMD5(gameSessionArr[instructions[1]].UserTwo)].sendUTF(`opPlay||${instructions[2]}`);
                        }
                        else if (conn.user === gameSessionArr[instructions[1]].UserTwo) {
                            connections[getStringMD5(gameSessionArr[instructions[1]].UserOne)].sendUTF(`opPlay||${instructions[2]}`);
                        }
                    }
                    else conn.sendUTF('err||' + result.msg);
                }
                break;
            //endRound&&sessionKey
            case 'endround':
                if (!gameSessionArr[instructions[1]]) {
                    conn.sendUTF('err||Requested session invalid!');
                    return;
                }
                else {
                    let result = gameSessionArr[instructions[1]].stopRound(conn.user);
                    if (result.status) {
                        conn.sendUTF('endRound||Success');
                        if (conn.user === gameSessionArr[instructions[1]].UserOne) {
                            connections[getStringMD5(gameSessionArr[instructions[1]].UserTwo)].sendUTF(`opEndRound`);
                        }
                        else if (conn.user === gameSessionArr[instructions[1]].UserTwo) {
                            connections[getStringMD5(gameSessionArr[instructions[1]].UserOne)].sendUTF(`opEndRound`);
                        }
                    }
                    else conn.sendUTF('err||' + result.msg);
                }
                break;
             //attackPt&&sessionKey&&username
            case 'attackpt':
                if (!gameSessionArr[instructions[1]]) {
                    conn.sendUTF('err||Requested session invalid!');
                    return;
                }
                else {
                    if (gameSessionArr[instructions[1]].UserOne === instructions[2]) {
                        let msg = `attackPt||${instructions[2]}||`;
                        let ground = gameSessionArr[instructions[1]].ground1;
                        msg += `${ground.NormalAttack}||${ground.EpicAttack}||${ground.ExoticAttack}||${ground.TotalAttck}`;
                        conn.sendUTF(msg);
                    }
                    else if (gameSessionArr[instructions[1]].UserTwo === instructions[2]) {
                        let msg = `attackPt||${instructions[2]}||`;
                        let ground = gameSessionArr[instructions[1]].ground2;
                        msg += `${ground.NormalAttack}||${ground.EpicAttack}||${ground.ExoticAttack}||${ground.TotalAttck}`;
                        conn.sendUTF(msg);
                    }
                    else {
                        conn.sendUTF('err||Requested User is not in this session!');
                    }
                }
                break;
            //getStatus&&sessionKey
            case 'getstatus':
                if (!gameSessionArr[instructions[1]]) {
                    conn.sendUTF('err||Requested session invalid!');
                    return;
                }
                else {
                    let returnMsg = `status`;
                    returnMsg += `||${gameSessionArr[instructions[1]].wins1}`;
                    returnMsg += `||${gameSessionArr[instructions[1]].wins2}`;
                    returnMsg += `||${gameSessionArr[instructions[1]].deckOne.length}`;
                    returnMsg += `||${gameSessionArr[instructions[1]].deckTwo.length}`;
                    conn.sendUTF(returnMsg);
                }
                break;
            //getGround&&sessionKey&&username
            case 'getground':
                if (!gameSessionArr[instructions[1]]) {
                    conn.sendUTF('err||Requested session invalid!');
                    return;
                }
                else {
                    if (gameSessionArr[instructions[1]].UserOne === instructions[2]) {
                        let msg = `ground||${instructions[2]}||`;
                        let ground = gameSessionArr[instructions[1]].ground1;
                        msg += gameSessionArr[instructions[1]].stop1 ? "true" : "false";
                        msg += `${ground.normalWeight}||${ground.epicWeight}||${ground.exoticWeight}`;
                        for (let card of ground.normals) {
                            msg += `||${card.cardIndex}`;
                        }
                        for (let card of ground.epics) {
                            msg += `||${card.cardIndex}`;
                        }
                        for (let card of ground.exotics) {
                            msg += `||${card.cardIndex}`;
                        }
                        conn.sendUTF(msg);
                    }
                    else if (gameSessionArr[instructions[1]].UserTwo === instructions[2]) {
                        let msg = `ground||${instructions[2]}||`;
                        let ground = gameSessionArr[instructions[1]].ground2;
                        msg += gameSessionArr[instructions[1]].stop2 ? "true" : "false";
                        msg += `${ground.normalWeight}||${ground.epicWeight}||${ground.exoticWeight}`;
                        for (let card of ground.normals) {
                            msg += `||${card.cardIndex}`;
                        }
                        for (let card of ground.epics) {
                            msg += `||${card.cardIndex}`;
                        }
                        for (let card of ground.exotics) {
                            msg += `||${card.cardIndex}`;
                        }
                        conn.sendUTF(msg);
                    }
                    else {
                        conn.sendUTF('err||Requested User is not in this session!');
                    }
                }
                break;
            //update&&sessionKey&&rickType&&x&&y
            case 'update':
                if (conn.id !== instructions[1]) {
                    conn.sendUTF('err||Invalid Request sender!');
                    return;
                }
                if (instructions.length !== 5) {
                    conn.sendUTF('err||Request parameters invalid!');
                    return;
                }
                else {
                    let loc = {
                        x: instructions[3],
                        y: instructions[4],
                        rickType: instructions[2]
                    };
                    if (!userLocations[conn.user]) {
                        for (let key in connections) {
                            let userConn = connections[key];
                            if (userConn.user !== conn.user) {
                                userConn.sendUTF(`newUsr||${conn.user}||${loc.rickType}||${loc.x}||${loc.y}`);
                            }
                        }
                    }
                    userLocations[conn.user] = loc;
                    for (let key in connections) {
                        let userConn = connections[key];
                        if (userConn.user !== conn.user) {
                            userConn.sendUTF(`newLoc||${conn.user}||${loc.rickType}||${loc.x}||${loc.y}`);
                        }
                    }
                }
                break;
            //allLoc
            case 'allloc':
                let msg = 'locs';
                for (let user in userLocations) {
                    let loc = userLocations[user];
                    msg += `||${user}||${loc.rickType}||${loc.x}||${loc.y}`;
                }
                conn.sendUTF(msg);
                break;
            default:
                conn.sendUTF('err||Wrong request!');
                break;
        }
    });

    conn.on('close', async function(code, reason){
        console.log("Close: " + conn.user);
        delete connections[conn.id];
    });

    conn.on('error', async function(code, reason){
        console.log("Error: " + conn.user);
        console.log(code);
        console.log(reason);
    });
});



function getStringMD5(str) {
    var md5 = crypto.createHash('md5');
    return md5.update(str).digest('hex');
}