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

app.use(router.routes());

app.listen(17777);
console.log('The Blog Server running on port 17777....');


/* ------  WebSocket Connection Server  ------ */
var connections = {};
let gameSessionArr = [];

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
                    conn.user = instructions[1];
                    var connectionID = getStringMD5(instructions[1]);
                    conn.send(connectionID);
                    connections[connectionID] = conn;
                    console.log('Created ws: ' + connectionID);
                }
                break;
            default:
                conn.send('Wrong request!' + conn.user);
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