/*
* Model.js
*/
'use strict';
const mysql = require('mysql');
const fs = require('fs');
const crypto = require('crypto');
const DBuser = require('./ConnectionInfo')

const connection = mysql.createConnection(DBuser.user);

connection.connect();

// Query sentences definition
// 1. Users
var allUser = 'SELECT * FROM users';
var selectUser = 'SELECT * FROM users WHERE username = ?';
var insertUser = 'INSERT INTO users(username, password, userImageUrl, phone, email) VALUES(?,?,?,?,?)';
var updateUser = 'UPDATE users SET phone = ?, email = ? WHERE username = ?';
var updateUser2 = 'UPDATE users SET phone = ?, email = ?, userImageUrl = ? WHERE username = ?';

// 5. Friends
var insertFriendShip = 'INSERT INTO friends(userOne, userTwo) VALUES(?,?)';
var selectFriends = 'SELECT * FROM friends WHERE userOne = ? AND userTwo = ?';
var myFriends = 'SELECT * FROM friends WHERE userOne = ? OR userTwo = ?';

exports.InsertUser = function(username, pass, userImageUrl, phone, email){
    /*
    *  -3 for username already exists, -2 for rePass not equal to pass,
    *  -1 for phone already exists, 0 for email alreadt exists,
    *  1 for success
    */
    var userParam = [username, pass, userImageUrl, phone, email];
    connection.query(insertUser, userParam, function(err, result){
        if(err) throw err;
    });
}

exports.AllUser = async function(){
    return new Promise((resolve, reject)=>{
        connection.query(allUser, function(err, result){
            if(err){
                reject(err);
            }else {
                resolve(result);
            }
        });
    });
}

exports.TestLogIn = async function(username, testPass){
    return new Promise((resolve, reject)=>{
        connection.query(allUser, function(err, result){
            var code = -1; // No such user
            if(err){
                reject(err);
            }else {
                result.forEach(function(user) {
                    if(user.username === username){
                        if(testPass == GetDecode_SHA256(user.password)){
                            code = 1; // Success
                        }else code = 0; // Password error
                    }
                });
                resolve(code);
            }
        });
    });
}

exports.GetUserInfo = async function(username){
    return new Promise((resolve, reject)=>{
        var selectParam = [username];
        var res = {};
        connection.query(selectUser, selectParam, function(err, result){
            if(err){
                reject(err);
            }
            if(result.length > 0){
                res['phone'] = result[0].phone;
                res['email'] = result[0].email;
                res['image'] = result[0].userImageUrl;
            }
            resolve(res);
        });
    });
}

exports.StoreUserImg = function(userImage, imageUrl){
    var buff = new Buffer(userImage, 'ascii');
    var trueImageUrl = 'static/img/' + imageUrl;
    fs.writeFileSync(trueImageUrl, userImage);
}

exports.SelectFriendsByName = function(username){
    return new Promise((resolve, reject) => {
        var res = [];
        connection.query(myFriends, [username, username], function(err, result){
            if(err){
                reject(err);
            }
            result.forEach(function(item){
                if(item.userOne == username)
                    res.push(item.userTwo);
                else
                    res.push(item.userOne);
            });
            resolve(res);
        });
    });
}

exports.AreFriends = async function(user1, user2){
    return new Promise((resolve, reject) => {
        var res = {
            'code': 0
        };
        connection.query(selectFriends, [user1, user2], function(err, result){
            if(err) reject(err);
            if(Object.keys(result).length > 0){
                res.code = 1;
                resolve(res);
            }else {
                connection.query(selectFriends, [user2, user1], function(err, result){
                    if(err) reject(err);
                    if(Object.keys(result).length > 0){
                        res.code = 1;
                    }
                    resolve(res);
                });
            }
        });
    });
}

exports.GetEncode_SHA256 = function(str){
    var secret = 'HoShiNoGen', key = secret.toString('hex');
    var cipher = crypto.createCipher('aes192', key);
    var encode_result = cipher.update(str, 'utf-8', 'hex');
    encode_result += cipher.final('hex');
    return encode_result;
}

var GetDecode_SHA256 = function(str){
    var secret = 'HoShiNoGen', key = secret.toString('hex');
    var decipher = crypto.createDecipher('aes192', key);
    var decode_result = decipher.update(str, 'hex', 'utf-8');
    decode_result += decipher.final('utf-8');
    return decode_result;
}