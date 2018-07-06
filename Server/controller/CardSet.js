let fs = require('fs');

//Global Identifiers
const NORMAL = 0;
const EPIC = 1;
const EXOTIC = 2;

function Card(index, type, attack, name) {
        this.cardIndex = index;
        this.type = type;
        this.attack = attack;
        this.cardName = name;
}

class CardSet {
    constructor() {
        //Identifiers of cards
        this.indexes = [];
        //Array of Card object
        this.collection = [];
        this.normals = [];
        this.epics = [];
        this.exotics = [];
        let collectionData = JSON.parse(fs.readFileSync('./controller/cardset.json', 'utf-8'));
        for (let card of collectionData) {
            let type;
            switch (card.type.toLowerCase()) {
                case "normal":
                    type = NORMAL;
                    this.normals.push(card);
                    break;
                case "epic":
                    type = EPIC;
                    this.epics.push(card);
                    break;
                case "exotic":
                    type = EXOTIC;
                    this.exotics.push(card);
                    break;
                default:
                    throw "Unknown Card type: " + card.type;
                    break;
            }
            this.indexes.push(card.index);
            this.collection.push(new Card(card.index, card.type, card.attack, card.name));
        }
    }

    getRandomSet(userSet) {
        let result = [];
        //Get 8 normals
        for (let i = 0; i < 8; i++) {
            let randIndex = Math.floor(Math.random() * this.normals.length);
            let cardIndex = this.normals[randIndex].cardIndex;
            while (userSet.indexOf(cardIndex) !== -1 || result.indexOf(cardIndex) === -1) {
                randIndex = Math.floor(Math.random() * this.normals.length);
                cardIndex = this.normals[randIndex].cardIndex;
            }
            result.push(cardIndex);
        }

        //Get 3 Epics
        for (let i = 0; i < 3; i++) {
            let randIndex = Math.floor(Math.random() * this.epics.length);
            let cardIndex = this.epics[randIndex].cardIndex;
            while (userSet.indexOf(cardIndex) !== -1 || result.indexOf(cardIndex) === -1) {
                randIndex = Math.floor(Math.random() * this.epics.length);
                cardIndex = this.epics[randIndex].cardIndex;
            }
            result.push(cardIndex);
        }

        //Get 2 Exotics
        for (let i = 0; i < 2; i++) {
            let randIndex = Math.floor(Math.random() * this.exotics.length);
            let cardIndex = this.exotics[randIndex].cardIndex;
            while (userSet.indexOf(cardIndex) !== -1 || result.indexOf(cardIndex) === -1) {
                randIndex = Math.floor(Math.random() * this.exotics.length);
                cardIndex = this.exotics[randIndex].cardIndex;
            }
            result.push(cardIndex);
        }

    }
}

exports.set = new CardSet();