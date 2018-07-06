/*
* Controller.js
*/
'use strict'
const model = require('../model/model.js');

exports.Login = async function(username, pass){
    var result = {
        'code' : 0, // 0 for failure, 1 for success
        'errMessage' : ''
    };
    // -1 for user not exit, 0 for password error, 1 for success
    await model.TestLogIn(username, pass).then(code=>{
        switch(code){
            case -1: 
                result['errMessage'] = 'User not exist!';
                break;
            case 0:
                result['errMessage'] = 'Wrong Password!';
                break;
            case 1:
                result['code'] = 1;
        }
    }); 
    return result;
}

exports.Logup = async function(username, pass, rePass, userImage, imgType, phone, email){
    var res = {
        'code' : 0,
        'errMessage' : ''
    };
    var resultCode = [];
    await model.AllUser().then(result=>{
        result.forEach(function(user) {
            if(user.username === username){
                resultCode.push(-3);
            }
            if(user.phone === phone){
                resultCode.push(-1);
            }
            if(user.email === email){
                resultCode.push(0);
            }
        });
        if(pass !== rePass){
            resultCode.push(-2);
        }
        // If no error items, then push success flag, insert user
        if(resultCode.length === 0){
            resultCode.push(1);
            model.InsertUser(username, model.GetEncode_SHA256(pass), username+imgType, phone, email);
        }
    });

    resultCode.forEach(function(item){
        switch(item){
            case -3:
                res['errMessage'] += 'Username already exists! ';
                break;
            case -1:
                res['errMessage'] += 'Phone number already exists! ';
                break;
            case 0:
                res['errMessage'] += 'E-mail already exist! ';
                break;
            case 1:
                res['code'] = 1;
                var imageUrl = username + imgType;
                if(userImage !== ""){
                    model.StoreUserImg(userImage, imageUrl);
                }
            }
    });
    return res;
}

exports.Data = async function(id, ctx){
    var userTest = false;
    if (ctx.session) {
        var currUser = ctx.session.user, currPass = ctx.session.pass;
        console.log(currUser);
        userTest = await model.TestLogIn(currUser, currPass);
        console.log(userTest);
    }
    var res = {
        'code' : 0, // Boolean
        'editor' : null,
        'title' : null,
        'contentData' : null, // String
        'imageUrl' : null, // String
        'mediaUrl' : null, // String
        'thumbsNum' : 0, // Int
        'comments' : {} // JSON, key: comment User name, value: comment message
    };
    if(userTest == 1){
        res['code'] = 1;
        var data = await model.ViewSinglePost(id);
        res.contentData = data[0]; 
        res.imageUrl = data[1];
        res.mediaUrl = data[2];
        res.editor = data[3];
        res.title = data[4];
        res.thumbsNum = data[5];
        res.comments = data[6];
    }
    return res;
}

exports.Upload = async function(username, pass, title, content, imageStream, imageType, mediaStream, mediaType){
    var test = await model.TestLogIn(username, pass);
    var errMess = '';
    switch(test){
        case -1: 
            errMess = 'User not exists!';
            break;
        case 0:
            errMess = 'Password error!';
    }
    var res = {
        'code' : 0,
        'errMessage' : errMess
    };
    if(test === 1){
        res['code'] = 1;
        var imageUrl, mediaUrl;
        var arr = await model.GetPostsIDs();
        var now_id = arr.length + 1;
        if(imageType === ''){
            imageUrl = '';
        }else {
            imageUrl =  now_id + '-' + username + '-' + title + imageType;
        }
        if(mediaType === ''){
            mediaUrl = '';
        }else {
            mediaUrl = now_id + '-' + username + '-' + title + mediaType;
        }

        if(imageStream !== null){
            model.StorePostImg(imageUrl, imageStream);
        }
        if(mediaStream !== null){
            model.StorePostMedia(mediaUrl, mediaStream);
        }
        model.InsertPost(title, username, content, imageUrl, mediaUrl);
    }
    return res;
}

exports.GetPostIDs = async function(){
    var res = await model.GetPostsIDs();
    return res;
}

exports.GiveThumbUp = async function(thumbUser, pass, postID){
    var test = await model.TestLogIn(thumbUser, pass);
    var res = {
        'code' : 0,
        'errMessage' : ''
    }
    if(test === 1){
        res['code'] = 1;
        model.GiveThumbUp(thumbUser, postID);
    }else {
        res['errMessage'] = 'Invalid User name or Password Error!';
    }
    return res;
}

exports.DeleteThumbUp = async function(thumbUser, pass, postID){
    var test = await model.TestLogIn(thumbUser, pass);
    var res = {
        'code' : 0,
        'errMessage' : ''
    }
    if(test === 1){
        res['code'] = 1;
        model.DeleteThumbUp(thumbUser, postID);
    }else {
        res['errMessage'] = 'Invalid User name or Password Error!';
    }
    return res;
}

exports.LeaveComment = async function(commentUser, pass, postID, content){
    var test = await model.TestLogIn(commentUser, pass);
    var res = {
        'code' : 0,
        'errMessage' : ''
    }
    if(test === 1){
        res['code'] = 1;
        model.LeaveMessage(commentUser, postID, content);
    }else {
        res['errMessage'] = 'Invalid User name or Password Error!';
    }
    return res;
}

exports.ModifyInfo = async function(username, pass, newUserImage, imgType, newPhone, newEmail){
    var test = await model.TestLogIn(username, pass);
    var res = {
        'code' : 0,
        'errMessage' : ''
    }
    if(test === 1){
        res['code'] = 1;
        model.ModifyUserInfo(username, newUserImage, imgType, newPhone, newEmail);
    }else {
        res['errMessage'] = 'Invalid User name or Password Error!';
    }
    return res;
}

exports.GetInfo = async function(username){
    var jsonBack = {};
    var data = await model.GetUserInfo(username);
    if(Object.keys(data).length > 0){
        jsonBack = {
            'code' : 1,
            'phone' : data['phone'],
            'email' : data['email'],
            'imageUrl' : data['image'],
            'username' : username,
            'errMessage' : ''
        };
    }else {
        jsonBack = {
            'code' : 0,
            'errMessage' : 'User not exist!'
        }
    }
    return jsonBack;
}

exports.GetAllPosts = async function(username, password){
    var test = await model.TestLogIn(username, password);
    var res = {
        'code' : 0,
        'errMessage' : ''
    }
    if(test === 1){
        res = await model.AllPosts();
        res['code'] = 1;
    }else {
        res['errMessage'] = 'Invalid User or Password!';
    }
    return res;
}

exports.checkThumbOrNot = async function(username, postID){
    var result = await model.checkThumbOrNot(username, postID);
    return result;
}

exports.AddComments = async function(username, id){
    var result = await model.AddComments(username, id);
    return result;
}

exports.SelectFriends = async function(username){
    var result = await model.SelectFriendsByName(username);
    var selectRes = {
        'friends': result
    };
    return selectRes;
}

exports.AreFriends = async function(user1, user2){
    var result = await model.AreFriends(user1, user2);
    return result;
}

exports.InsertEmail = async function(user1, user2){
    model.InsertEmail(user1, user2);
}

exports.DealEmail = async function(user1, user2, accept){
    model.DealEmail(user1, user2, accept);
}

exports.HaveEmail = async function(user1, user2){
    /*
    * checkRes: 1 for no email, 0 for you have send such email, 
    * -1 for target have send such email 
    */
    var res = {};
    var checkRes = await model.HaveEmail(user1, user2);
    var res = {
        'code': checkRes
    }
    return res;
}

exports.MyEmail = async function(username){
    var res = {
        'requestUser': []
    };
    var checkRes = await model.MyEmail(username);
    res.requestUser = checkRes;
    return res;
}

exports.GetTalks = async function(userSend, userGet){
    var res = await model.MyTalks(userSend, userGet);
    return res;
}

exports.InsertTalk = async function(userSend, userGet, content){
    model.InsertTalk(userSend, userGet, content);
}

exports.checkAvailID = async function(id){
    var allID = await model.GetPostsIDs();
    if(allID.indexOf(parseInt(id)) === -1)
        return false;
    else 
        return true;
}
