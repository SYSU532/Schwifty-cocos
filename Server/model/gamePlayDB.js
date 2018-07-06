'use strict';
const mysql = require('mysql');
const fs = require('fs');
const crypto = require('crypto');
const DBuser = require('./ConnectionInfo')

const connection = mysql.createConnection(DBuser.user);

connection.connect();

const checkTable = 'SELECT * FROM userDecks';
const initDBTable = 'CREATE TABLE IF NOT EXISTS `userDecks` (\n' +
    '  `username` varchar(100) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,\n' +
    '  `deckData` text NOT NULL\n' +
    ') ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;';
const addUnique = 'ALTER TABLE `userDecks`' +
    '  ADD KEY `username` (`username`);';
const addForeignKey = 'ALTER TABLE `userDecks`' +
    '  ADD CONSTRAINT `userDecks_ibfk_1` FOREIGN KEY (`username`) REFERENCES `users` (`username`) ON DELETE CASCADE ON UPDATE CASCADE;';

const selectDecks = 'SELECT deckData FROM userDecks WHERE username = ?;';
const updateDecks = 'UPDATE userDecks SET deckData = ? WHERE username = ?;';
const initUserDecks = 'INSERT INTO userDecks(username, deckData) VALUES(?, ?);';

connection.query(checkTable, [], function (err, res) {
    if (err) {
        connection.query(initDBTable, [], function (err, res) {
            if (err) throw err;
            console.log(res);
            connection.query(addUnique, [], function (err, res) {
                if (err) throw err;
                console.log(res);
                connection.query(addForeignKey, [], function (err, res) {
                    if (err) throw err;
                    console.log(res);
                });
            });
        });
    }
});

exports.QueryUserDecks = async function(username) {
    return new Promise(function (resolve, reject) {
        connection.query(selectDecks, [username], function (err, res) {
            if (err) reject(err);
            res.forEach(function (deck) {
                console.log(deck.deckData);
                resolve(JSON.parse(deck.deckData));
            });
            if (res.length === 0) {
                connection.query(initUserDecks, [username, JSON.stringify([...Array(24).keys()])], function (err, res) {
                    if (err) reject(err);
                    resolve([...Array(24).keys()]);
                })
            }
        })
    });
};


exports.UpdateUserDeck = async function(username, newDeck) {
    return new Promise(function (resolve, reject) {
        connection.query(updateDecks, [JSON.stringify(newDeck), username], function (err, res) {
            if (err) reject(err);
        })
    });
};

