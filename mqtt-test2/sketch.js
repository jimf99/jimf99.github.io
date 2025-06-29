/*
    p5.js MQTT Client example
    This example uses p5.js: https://p5js.org/
    and the Eclipse Paho MQTT client library: https://www.eclipse.org/paho/clients/js/
    to create an MQTT client that sends and receives MQTT messages.
    The client is set up for use on the shiftr.io test MQTT broker (https://shiftr.io/try),
    but has also been tested on https://test.mosquitto.org  
    created 12 June 2020
    modified 20 Aug 2020
    by Tom Igoe
*/  

// MQTT client details:
let broker = {

  hostname: 'public.cloud.shiftr.io',
  port: 443
};
// MQTT client:
let client;
// client credentials:
let creds = {
  clientID: 'p5Client', // choose whatever name you want
  userName: 'public', // unique Key from token
  password: 'public' // unique Secret from token
}
// topic to subscribe to when you connect:
let topic = 'arduino/simple';

// a pushbutton to send messages
let sendButton;
let localDiv;
let remoteDiv;
let inpt;

let input2
let sendButton2;
let localDiv2;
let remoteDiv2;

// intensity of the circle in the middle
let intensity = 255;

let leds = [];

function setup() {
  createCanvas(800, 400);

  for (let i = 0; i < 4; i++) {
    leds[i] = new Led(i * 50 + 20, height / 2, color(120,120,120), false);
  }
  // Create an MQTT client:
  client = new Paho.MQTT.Client(broker.hostname, Number(broker.port), creds.clientID);
  // set callback handlers for the client:
  client.onConnectionLost = onConnectionLost;
  client.onMessageArrived = onMessageArrived;
  // connect to the MQTT broker:
  client.connect({
    onSuccess: onConnect, // callback function for when you connect
    userName: creds.userName, // username
    password: creds.password, // password
    useSSL: true // use SSL
  });
  // create input
  // inpt = createInput("enter 0-255");
  // inpt.position(20, 0);
  // // create the send button:
  // sendButton = createButton('send to LED 1');
  // sendButton.position(20, 20);
  // sendButton.mousePressed(sendMqttMessage);
  // create a div for local messages:
  localDiv = createDiv('local messages will go here');
  localDiv.position(20, 50);
  localDiv.style('color', '#fff');
  // create a div for the response:
  remoteDiv = createDiv('waiting for messages');
  remoteDiv.position(20, 80);
  remoteDiv.style('color', '#fff');




  for (let i = 0; i < leds.length; i++) {
    if (i == 0) {
      leds[i].name = "ledR";
      leds[i].infill = color(255, 0, 0);
    }
    if (i == 1) {
      leds[i].name = "ledY";
      leds[i].infill = color(255, 255, 0);
    }
    if (i == 2) {
      leds[i].name = "ledG";
      leds[i].infill = color(0, 255, 0);
    }
    if (i == 3) {
      leds[i].name = "ledB";
      leds[i].infill = color(0, 125, 255);
    }
  }

}

function draw() {
  background(50);
  for (let i = 0; i < leds.length; i++) {
    leds[i].show();
    leds[i].detect();
  }
  // draw a circle whose brightness changes then a message is received:
  // fill(intensity);
  // circle(width / 2, height / 2, width / 2);
  // subtract one from the brightness of the circle:
  // if (intensity > 0) {
  //   intensity--;
  // }
}

function mousePressed() {
  for (let i = 0; i < leds.length; i++) {
    if(leds[i].detect()){
      leds[i].sendMsg();
    }
  }
}

// called when the client connects
function onConnect() {
  localDiv.html('client is connected');
  client.subscribe(topic);
  console.log("connected")
}

// called when the client loses its connection
function onConnectionLost(response) {
  if (response.errorCode !== 0) {
    localDiv.html('onConnectionLost:' + response.errorMessage);
  }
}

// called when a message arrives
function onMessageArrived(message) {
  remoteDiv.html('I got a message:' + message.payloadString);
  let incoming = split(trim(message.payloadString), "/");
  console.log(incoming);
  // let incomingNumber = parseInt(message.payloadString);
  // if (incomingNumber > 0) {
  //   intensity = incomingNumber;
  // }
}

// called when you want to send a message:
function sendMqttMessage() {
  // if the client is connected to the MQTT broker:
  if (client.isConnected()) {
    // make a string with a random number form 0 to 15:
    // let msg2 = String(round(random(15)));
    let msg2 = String(inpt.value());
    let msg = "ledG/" + msg2;
    // start an MQTT message:
    message = new Paho.MQTT.Message(msg);
    // choose the destination topic:
    message.destinationName = topic;
    // send it:
    client.send(message);
    // print what you sent:
    localDiv.html('I sent: ' + message.payloadString);
  }
}

// called when you want to send a message:
function sendMqttMessage2() {
  // if the client is connected to the MQTT broker:
  if (client.isConnected()) {
    // make a string with a random number form 0 to 15:
    //let msg2 = String(round(random(15)));
    let msg2 = String(input2.value());

    let msg = "ledY/" + msg2 + "\n";
    // start an MQTT message:
    message = new Paho.MQTT.Message(msg);
    // choose the destination topic:
    message.destinationName = topic;
    // send it:
    client.send(message);
    // print what you sent:
    localDiv2.html('I sent: ' + message.payloadString);
  }
}


class Led {
  constructor(x, y, fil, onOff) {
    this.x = x;
    this.y = y;
    this.infill = fil;
    this.size = 30;
    this.onOff = onOff;
    this.name = "";
  }
  sendMsg() {
    if (client.isConnected()) {
      // make a string with a random number form 0 to 15:
      // let msg2 = String(round(random(15)));
      this.onOff = !this.onOff;
      let out;
      if (this.onOff) {
        out = String(255);
      } else {
        out = String(0);
      }

      let msg = this.name + "/" + out;
      // start an MQTT message:
      let message;
      message = new Paho.MQTT.Message(msg);
      // choose the destination topic:
      message.destinationName = topic;
      // send it:
      client.send(message);
      // print what you sent:
      localDiv.html('I sent: ' + message.payloadString);
    }

  }

  show() {
    if (this.onOff) {
      fill(this.infill);
    } else {
      fill(120);
    }
    circle(this.x, this.y, this.size);
  }

  detect() {
    if (dist(mouseX, mouseY, this.x, this.y) <= this.size) {
      //fill(this.inFill);
      return true;

    } else {
    //  fill(120);
      return false;
    }
  }
}
