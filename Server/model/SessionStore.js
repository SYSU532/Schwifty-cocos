const session = require('koa-session-minimal');
const MysqlSession = require('./sessionDB');
const DBUser = require('./ConnectionInfo');
// class dbStore extends Store {
//     constructor() {
//         super();
//     }
//
//     async get(sid, ctx) {
//         return await await database.getSessionInfo(sid);
//     }
//
//     async set(session, { sid =  this.getID(24), maxAge = 86400000 } = {}, ctx) {
//         await await database.addSession(sid, JSON.stringify(session));
//         return sid;
//     }
//
//     async destroy(sid, ctx) {
//         await await database.dropSession(sid);
//     }
// }

exports.store = new MysqlSession(DBUser.user);