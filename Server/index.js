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
console.log('The Blog Server running on port 17777....');


/* ------  WebSocket Connection Server  ------ */
var connections = {};
let gameSessionArr = [];
let gameRequest = {};

var server = ws.createServer(function(conn){
    conn.on('text', async function(str){
        console.log(str);
        let instructions = str.split('&&');
        console.log(instructions);
        switch (instructions[0].toLowerCase()) {
            case 'login':
                if (instructions.length !== 3) {
                    conn.send("Invalid number of params");
                    return;
                }
                let code = await model.TestLogIn(instructions[1], instructions[2]);
                if (code === 1) {
                    if (connections[getStringMD5(instructions[1])]) {
                        conn.send('err||Already signed in!');
                        return;
                    }
                    conn.user = instructions[1];
                    var connectionID = getStringMD5(instructions[1]);
                    conn.id = connectionID;
                    conn.send(connectionID);
                    connections[connectionID] = conn;
                    console.log('Created ws: ' + connectionID);
                }
                break;
            case 'gamereq':
                if (connections[instructions[1]].id !== instructions[1]) {
                    conn.send('err||Invalid Request sender!');
                    return;
                }
                if (!connections[getStringMD5(instructions[2])]) {
                    conn.send('err||Requested User is not online!');
                    return;
                }
                let userDeck = await gameDB.QueryUserDecks(instructions[2]);
                let deckCount = userDeck.length;
                connections[getStringMD5(instructions[2])].send(`recReq||${connections[instructions[1]].user}||${deckCount}`);
                conn.send('reqPend||Request Sent');
                gameRequest[instructions[1]] = [getStringMD5(instructions[2])];
                break;
            case 'accreq':
                let targetUserMD5 = getStringMD5(instructions[2]);
                if (connections[instructions[1]].id !== instructions[1]) {
                    conn.send('err||Invalid Request User!');
                    return;
                }
                if (!connections[getStringMD5(instructions[2])]) {
                    conn.send('err||Sender User is not online!');
                    return;
                }
                if (!gameRequest[targetUserMD5] ||
                    gameRequest[targetUserMD5].indexOf(instructions[1]) === -1) {
                    conn.send('err||No relevant request found!');
                    return;
                }
                delete gameRequest[targetUserMD5];
                conn.send(`reqAc||${instructions[2]}`);
                connections[targetUserMD5].send(`reqAc||${connections[instructions[1]].user}`);
                gameSessionArr.push(new gamePlay.session(connections[instructions[1]].user, instructions[2]));
                break;
            default:
                conn.send('err||Wrong request!');
                break;
        }
    });

    conn.on('close', async function(code, reason){
        console.log(code);
        console.log(reason);
    });

    conn.on('error', async function(code, reason){
        console.log(code);
        console.log(reason);
    });
}).listen(17700);

console.log('WebSocket Server Running on port 17700....');



function getStringMD5(str) {
    var md5 = crypto.createHash('md5');
    return md5.update(str).digest('hex');
}