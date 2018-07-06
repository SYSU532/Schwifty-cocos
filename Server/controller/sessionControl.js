const session = require('koa-session-minimal');
const store = require('./../model/SessionStore').store;


let cookie = {
    maxAge: '', // cookie有效时长
    expires: '',  // cookie失效时间
    path: '', // 写cookie所在的路径
    domain: '', // 写cookie所在的域名
    httpOnly: '', // 是否只用于http请求中获取
    overwrite: '',  // 是否允许重写
    secure: '',
    sameSite: '',
    signed: '',

};



module.exports = {
    session: session({
        key: 'SESSION_ID',
        store: store,
        cookie: cookie
    })
}