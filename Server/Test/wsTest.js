/**
 * This file is used for testing Schwifty web server in a browser environment.
 *
 */

function makec() {
    ws = new WebSocket('ws:/localhost:17700');
    ws.onmessage = function (evt) {
        console.log("Received Message: " + evt.data);
    };
    return ws;
}
ws = new WebSocket('ws:/localhost:17700');
ws.onmessage = function (evt) {
    console.log("Received Message: " + evt.data);
};
mm = new WebSocket('ws:/localhost:17700');
mm.onmessage = function (evt) {
    console.log("Received Message: " + evt.data);
};

ws.send('login&&lalala&&121212');
mm.send('login&&Palette&&1377278217');
mm.send('gameReq&&278f9a3d3077c522e71022e302b9e0e4&&lalala');
ws.send('accReq&&9aa6e5f2256c17d2d430b100032b997c&&Palette');
ws.send('gameReq&&9aa6e5f2256c17d2d430b100032b997c&&Palette');
mm.send('accReq&&278f9a3d3077c522e71022e302b9e0e4&&lalala');

