var gateway = `ws://s{window.location.host}/ws`;
var websocket;

window.addEventListener("load", onLoad);

function onLoad(event) {
    initWebSocket();
}

function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage; 
}

function onOpen(event) {
    console.log('Connection opened');
}

function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
}

function onMessage(event) {
    var message = JSON.parse(event.data);
    if (message.id == 1) {
        document.getElementById("ledState").innerHTML = message.status == 1 ? "ON" : "OFF";
        document.getElementById("button1").classList.toggle('off', message.status == 0);
    } else if (message.id == 2) {
        document.getElementById("ledState2").innerHTML = message.status == 1 ? "ON" : "OFF";
        document.getElementById("button2").classList.toggle('off', message.status == 0);
    } else if (message.id == 3) {
        document.getElementById("ledState3").innerHTML = message.status == 1 ? "ON" : "OFF";
        document.getElementById("button3").classList.toggle('off', message.status == 0);
    } else if (message.id == 4) {
        document.getElementById("ledState4").innerHTML = message.status == 1 ? "ON" : "OFF";
        document.getElementById("button4").classList.toggle('off', message.status == 0);
    }
}

function toggleLED(ledId) {
    websocket.send(JSON.stringify({ id: ledId }));
}