const deckDB = require('./../model/gamePlayDB');
const cardSet = require('./CardSet');

//Global Identifiers
const NORMAL = 0;
const EPIC = 1;
const EXOTIC = 2;


/**
* Represent the current Card Table for one user.
 */
class Ground {
    constructor() {
        this.normals = [];
        this.epics = [];
        this.exotics = [];
        this.normalWeight = 1;
        this.epicWeight = 1;
        this.exoticWeight = 1;
    }

    get NormalAttack() {
        let sum = 0;
        for (let card of this.normals) {
            sum += card.attack;
        }
        sum *= this.normalWeight;
        return sum;
    }

    get EpicAttack() {
        let sum = 0;
        for (let card of this.epics) {
            sum += card.attack;
        }
        sum *= this.epicWeight;
        return sum;
    }

    get ExoticAttack() {
        let sum = 0;
        for (let card of this.exotics) {
            sum += card.attack;
        }
        sum *= this.exoticWeight;
        return sum;
    }

    get TotalAttack() {
        return this.NormalAttack + this.EpicAttack + this.ExoticAttack;
    }

    clear() {
        this.normals = [];
        this.epics = [];
        this.exotics = [];
        this.normalWeight = 1;
        this.epicWeight = 1;
        this.exoticWeight = 1;
    }

    addCard(cardIndex) {
        let newCard = cardSet.set.collection[cardIndex];
        switch (newCard.type) {
            case NORMAL:
                this.normals.push(newCard);
                break;
            case EPIC:
                this.epics.push(newCard);
                break;
            case EXOTIC:
                this.exotics.push(newCard);
                break;
            default:
                throw "Unknown type when adding to Card Ground: " + newCard.type;
                break;
        }
    }
}

class GameSession {
    constructor(hostUser, guestUser) {
        if (hostUser === guestUser ) {
            throw "Same user detected when creating GameSession!";
        }
        this.UserOne = hostUser;
        this.UserTwo = guestUser;
        this.currentPlayer = hostUser;
        // The current round of this session
        this.currentRound = 1;
        // The rounds that each player has won
        this.wins1 = 0;
        this.wins2 = 0;
        //Indicate whether the player has folded
        this.stop1 = false;
        this.stop2 = false;
        // The Card ground of each player, clear on each round
        this.ground1 = new Ground();
        this.ground2 = new Ground();
        this.initDeck();
    }

    async initDeck() {
        this.deckSetOne = await deckDB.QueryUserDecks(this.UserOne);
        this.deckSetTwo = await deckDB.QueryUserDecks(this.UserTwo);
        //Init usable deck
        this.deckOne = cardSet.set.getRandomSet(this.deckSetOne);
        this.deckTwo = cardSet.set.getRandomSet(this.deckSetTwo);
    }

    get Winner() {
        if (this.currentRound === 3 && this.wins1 === 2 && this.wins2 === 0) {
            return this.UserOne;
        }
        if (this.currentRound === 3 && this.wins2 === 2 && this.wins1 === 0) {
            return this.UserTwo;
        }
        if (this.wins2 === this.wins1 && this.currentRound > 3) {
            return "Draw";
        }
        if (this.currentRound > 3 && this.wins1 > this.wins2) {
            return this.UserOne;
        }
        if (this.currentRound > 3 && this.wins1 < this.wins2) {
            return this.UserTwo;
        }
        return "Unknown";
    }

    yield(username) {
        this.stop1 = false;
        this.stop2 = false;
        if (this.UserOne === username) {
            this.wins2++;
        }
        else if (this.UserTwo === username) {
            this.wins1++;
        }
        this.currentRound++;
        this.ground1.clear();
        this.ground2.clear();
    }

    endRound() {
        this.stop1 = false;
        this.stop2 = false;
        if (this.ground2.TotalAttack >= this.ground1.TotalAttack) {
            this.wins2++;
        }
        if (this.ground1.TotalAttack >= this.ground2.TotalAttack) {
            this.wins1++;
        }
        console.log(`Round end, ${this.ground1.TotalAttack}, ${this.ground2.TotalAttack}`);
        this.currentRound++;
        this.ground1.clear();
        this.ground2.clear();
    }

    stopRound(username) {
        if (this.UserOne === username) {
            this.stop1 = true;
            if (this.stop2) {
                this.endRound();
                return {status: true, nextRound: true};
            }
            this.currentPlayer = this.UserTwo;
            return {status: true, nextRound: false};
        }
        else if (this.UserTwo === username) {
            this.stop2 = true;
            if (this.stop1) {
                this.endRound();
                return {status: true, nextRound: true};
            }
            this.currentPlayer = this.UserOne;
            return {status: true, nextRound: false};
        }
        else console.log(`Unexpected User: ${this.currentPlayer}, ${this.UserOne}, ${this.UserTwo}`);
        return {status: false, msg: 'Wrong player'};
    }

    playCard(username, cardIndex) {
        cardIndex = parseInt(cardIndex);
        if (this.currentRound > 3 || this.Winner !== "Unknown") {
            return {status: false, msg: 'Session is already over.'};
        }
        if (this.currentPlayer === this.UserOne && this.UserOne === username) {
            if (this.deckOne.indexOf(cardIndex) === -1) {
                return {status: false, msg: 'Player does not have the card.'}
            }
            this.ground1.addCard(cardIndex);
            if (!this.stop2)
                this.currentPlayer = this.UserTwo;
            this.deckOne.splice(this.deckOne.indexOf(cardIndex), 1);
            return {status: true};
        }
        else if (this.currentPlayer === this.UserTwo && this.UserTwo === username) {
            if (this.deckTwo.indexOf(cardIndex) === -1) {
                return {status: false, msg: 'Player does not have the card.'}
            }
            this.ground2.addCard(cardIndex);
            if (!this.stop1)
                this.currentPlayer = this.UserOne;
            this.deckTwo.splice(this.deckTwo.indexOf(cardIndex), 1);
            return {status: true};
        }
        else console.log(`Unexpected Card Play: ${this.currentPlayer}, ${this.UserOne}, ${this.UserTwo}`);
        return {status: false, msg: 'Wrong player'};
    }

}

exports.session = GameSession;