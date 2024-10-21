let img;

let mBounce = 0;
let mDown = 0;
let frameCtr = 0;
let msg = "";
let keypadXpos = 2 * 8;
let keypadYpos = 20 * 8;
let drawKeypad = 1;
let ky = "";
let msgTimer = 0;

var rxMsg = "";
var message="";
var x=0;
var y=0;
var rw=0;
var col=0;

function setup1() {
  frameRate(60);
  createCanvas(600, 480);
  img = loadImage('/data/keypad.jpg');
}

function milSec(prevMs) {
  return millis() - prevMs;
}

function updateKeypad() {
  if (drawKeypad == 0) {
    return;
  }
  image(img, keypadXpos, keypadYpos, 300, 300);
}

function showFrameCtr() {
  textSize(32);
  fill(color(255,255,255));
  text(str(round(frameCtr/100)),18*16,5*32);
}

function updateRowCol() {
  var sc=10;
  var ctr=0;
    for(y=0;y<10;y++) {
      fill(color(0,255,255));
      var xPos=320;
      var yPos=200+(++ctr)*sc;
      rect(xPos,yPos,sc,sc);
      rect(xPos,yPos+4,400,2);
  }
  if(rw==0 && col==0 && rxMsg>"") {
    var tmp=JSON.parse(rxMsg);
    tmp=tmp.txt+" . ";
    var words=split(tmp," ");
    words[1]=words[1]+"0000";
    if(words.length>0) {
      console.log(words);
      if(words[1][0]=="C") {
        col=int(words[1][1]);
        console.log("col="+str(col));
      }
      if(words[1][2]=="R") {
        rw=int(words[1][3]);
        console.log("rw="+str(rw));
      }
      }
    }
  if(rw>0 && col>0) {
    fill(color(0,255,0));
    rect(330,200+(col)*sc,sc,sc);
    fill(color(0,255,0));
    rect(330,200+(rw+6)*sc,sc,sc);
  }
}

function draw() {
  frameCtr = ++frameCtr % 32767;
  if (rxMsg > "") {
    tmp = JSON.parse(rxMsg);
    msg = tmp.txt;
  }
  if (msgTimer > 0) {
    msgTimer = msgTimer - 1;
  }
  if (msg > "" && msgTimer == 0) {
    msg = "";
    rxMsg = "";
    rw=0;
    col=0;
  }
  background(color(16, 32, 16));
  updateKeypad();
  showFrameCtr();
  updateRowCol();
if (rxMsg > "") {
    tmp = JSON.parse(rxMsg);
    x=tmp.x;
    y=tmp.y;
    fill(color(0,255,0));
    circle(x,y,20);
  }
  ky = "";
  if (mDown == 2 && mBounce >= 250) {
    msg="";
    rxMsg="";
    let dX = 10;
    let mX = mouseX - keypadXpos;
    let mY = mouseY - keypadYpos;
    if (dist(47, 27, mX, mY) <= dX) {
      ky = "F1 C6R4";
    }
    if (dist(115, 30, mX, mY) <= dX) {
      ky = "F2 C6R3";
    }
    if (dist(183, 30, mX, mY) <= dX) {
      ky = "F3 C6R2";
    }
    if (dist(251, 30, mX, mY) <= dX) {
      ky = "F4 C6R1";
    }
    if (dist(34, 87, mX, mY) <= dX) {
      ky = "1 C1R1";
    }
    if (dist(90, 87, mX, mY) <= dX) {
      ky = "2 C2R1";
    }
    if (dist(137, 87, mX, mY) <= dX) {
      ky = "3 C3R1";
    }
    if (dist(31, 147, mX, mY) <= dX) {
      ky = "4 C1R2";
    }
    if (dist(83, 147, mX, mY) <= dX) {
      ky = "5 C2R2";
    }
    if (dist(141, 147, mX, mY) <= dX) {
      ky = "6 C3R2";
    }
    if (dist(26, 195, mX, mY) <= dX) {
      ky = "7 C1R3";
    }
    if (dist(85, 195, mX, mY) <= dX) {
      ky = "8 C2R3";
    }
    if (dist(137, 195, mX, mY) <= dX) {
      ky = "9 C3R3";
    }
    if (dist(24, 257, mX, mY) <= dX) {
      ky = ". C1R4";
    }
    if (dist(80, 262, mX, mY) <= dX) {
      ky = "0 C2R4";
    }
    if (dist(137, 262, mX, mY) <= dX) {
      ky = "- C3R4";
    }
    if (dist(195, 85, mX, mY) <= dX) {
      ky = "UP C4R1";
    }
    if (dist(195, 145, mX, mY) <= dX) {
      ky = "DOWN C4R2";
    }
    if (dist(195, 206, mX, mY) <= dX) {
      ky = "LEFT C4R3";
    }
    if (dist(195, 265, mX, mY) <= dX) {
      ky = "RIGHT C4R4";
    }
    if (dist(257, 88, mX, mY) <= dX) {
      ky = "ENTER C5R1";
    }
    if (dist(259, 143, mX, mY) <= dX) {
      ky = "BACK C5R2";
    }
    if (dist(259, 202, mX, mY) <= dX) {
      ky = "MENU C5R3";
    }
    if (dist(260, 263, mX, mY) <= dX) {
      ky = "ESCAPE C5R4";
    }
    // msg=str(mX)+" "+str(mY)+" "+ky;
    if (ky == "") {
      ky = "xx";
    }
    console.log(msg + " " + ky[0] + ky[1]);
    mBounce = milSec(0);
    var ch = "keypad/" + ky;
    var message = {
      ts: Date.now(),
      x: round(mouseX,4),
      y: round(mouseY,4),
      txt: ch + "" + msg,
    };
    rw=0;
    col=0;
    client.publish(topic, JSON.stringify(message));
  }
  textSize(32);
  fill(color(255, 255, 255));
  text(msg, 32, 1 * 32);
  mDown = 0;
}

function mousePressed() {
  mDown = 1;
  mBounce = milSec(0);
}

function mouseDragged() {
  // mDown+=1;
}

function mouseReleased() {
  mDown = 2;
}

/******************
Code inspired by Vamoss
Source: https://openprocessing.org/sketch/1440432
github: http://github.com/vamoss
******************/

var client;
var topic = "jimfyyc/mqtt/p5js";
var broker = "wss://test.mosquitto.org:8081";
var prevX = 0;
var prevY = 0;


function setup() {
  setup1();
  client = mqtt.connect(broker);
  client.subscribe(topic, function (err) {
    console.log("mqtt subscribed");
    if (err) console.error(err);
  });

  client.on("connect", function (connack) {
    console.log("mqtt connected", connack.sessionPresent);
  });

  client.on("message", function (topic, payload) {
    console.log(topic + " " + payload.toString());
    message = JSON.parse(payload.toString());
    x = Math.floor(message.x);
    y = Math.floor(message.y);
    console.log(str(x)+" "+str(y));
    msgTimer = 120;
    rxMsg = payload.toString();
  });
}

function keyPressed() {
  ky = keyCode;
}
