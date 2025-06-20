/*
  JimF99
  Started: 19 June 2025
  Updated: 19 June 2025
  Special Thanks to Tom Igo
*/

let broker = {
    hostname: 'public.cloud.shiftr.io',
    port: 443
};

let client;

let creds = {
    clientID: 'p5Client_MIDI_TEST',
    userName: 'public',
    password: 'public'
}

let topic = 'noteMIDI';

let sendButton;
let localDiv;
let remoteDiv;

let intensity = 255;

function setup() {
    createCanvas(300, 400);
    client = new Paho.MQTT.Client(broker.hostname, Number(broker.port), creds.clientID);
    client.onConnectionLost = onConnectionLost;
    client.onMessageArrived = onMessageArrived;
    client.connect(
        {
            onSuccess: onConnect,
            userName: creds.userName,   // username
            password: creds.password,   // password
            useSSL: true                // use SSL
        }
    );
  
    sendButton = createButton('Button1');
    sendButton.position(20, 20);
    sendButton.mousePressed(sendMqttMessage1);
  
    localDiv = createDiv('local messages will go here');
    localDiv.position(20, 50);
    localDiv.style('color', '#fff');

    remoteDiv = createDiv('Local ...');
    remoteDiv.position(20, 80);
    remoteDiv.style('color', '#fff');
     
    sendButton = createButton('Button2');
    sendButton.position(100, 20);
    sendButton.mousePressed(sendMqttMessage2);
    
  
    remoteDiv = createDiv('Remote ...');
    remoteDiv.position(20, 100);
    remoteDiv.style('color', '#fff');
}

function draw() {
    background(50);
    fill(intensity);
    circle(width/2, height/2, width/2);
    if (intensity > 0) {
        intensity--;
    }
}

function onConnect() {
    localDiv.html('Connected!');
    client.subscribe(topic);
}

function onConnectionLost(response) {
    if (response.errorCode !== 0) {
        localDiv.html('onConnectionLost:' + response.errorMessage);
    }
}

function onMessageArrived(message) {
    remoteDiv.html('Received: ' + message.payloadString);
    let  incomingNumber = parseInt(message.payloadString);
    if (incomingNumber > 0) {
        intensity = 255;
    }
}

function sendMqttMessage1() {
    if (client.isConnected()) {
        let msg = String(3)+",Button1";
        message = new Paho.MQTT.Message(msg);
        message.destinationName = topic;
        client.send(message);
        localDiv.html('Button1 sent: ' + message.payloadString);
    }
}

function sendMqttMessage2() {
    if (client.isConnected()) {
        let msg = String(33)+",Button2";
        message = new Paho.MQTT.Message(msg);
        message.destinationName = topic;
        client.send(message);
        localDiv.html('Button 2 sent: ' + message.payloadString);
    }
}
