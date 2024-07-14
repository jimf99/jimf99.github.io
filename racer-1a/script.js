/* http-server -c-1
*/

let myColor = [100, 100, 100];

var player1;
var cone1;
var drawCones=false;

var walls;
var Cans;
var cans9=false;
var drawTrack=true;


let xoff = 0;
let yoff = 0;
let xsc = 25;
let ysc = 25;
let canvWidth = xsc * 50;
let canvHeight = ysc * 24;
let gameRound = 0;

let doNewCans = 1;
let objs = new Array();

let plife = 30;
let numPlayers = 1;
var recData;

let xmousePressed = 0;
let jmouseX1 = 0;
let jmouseY1 = 0;
let thY = 0;
let head1 = 0;


function newCans() {
 if(cans9==false) {return;}
  for (i = 0; i < 9; i++) {
    let row = random(5);
    let col = i + 1;
    let x1 = 100 + (xsc * 3) + (col * 2) * xsc * 1.5;
    let y1 = 100 + (row + 1) * ysc * 2 + (ysc);
    theCan = createCan(x1, y1, color(255, 165, 0), "CAN");
    let thePos = {};
    thePos.x = x1;
    thePos.y = y1;
    objs[i] = thePos;
  }
  doNewCans = 0;
  gameRound += 1;
}

function createCan(x, y, c, id) {
  let csc = 40;
  let Can = createSprite(x, y, csc, csc);
  Can.shapeColor = c;
  Cans.add(Can);
  Can.draw = function() {
    fill(this.shapeColor);
    ellipse(0, 0, csc, csc);
    fill("black");
    textAlign(CENTER, CENTER);
    textSize(12);
    text(id, 0, 0);
  }

  // Can.setCollider("circle", 0, 0, csc/2);

  return Can;
}

var player1StartX = 0;
var player1StartY = 0;

var timeleft = 300;

var startTime = 0;
var currentTime = 0;

let timer, interval;
var timerStr;

function convertSeconds(s) {
  var min = floor(s / 60);
  var sec = s % 60;
  return nf(min, 2) + ':' + nf(sec, 2);
}

var car1;
var car2;
var track1;
var logo1;


var tracks = new Array();
tracks.push('tracks/reinvent2019_wide.svg');
tracks.push('tracks/2022_october_open.svg');

function setup() {
  car1=loadImage("car.png");
  logo1=loadImage("logo-001a.png");
  cone1=loadImage("cone-001a.png");
  track1=loadImage(tracks[0]);
  createCanvas(canvWidth, canvHeight);
  background(100);

  angleMode(DEGREES);
  var xPos = 3.3 * width / 4;
  var yPos = 2.0 * height / 4

  player1 = getTank(xPos, yPos, "blue");
  player1.rotation = 180;

  walls = new Group();
  Cans = new Group();

  textAlign(CENTER);

  strokeWeight(2);
  stroke("black");


   theWall = createWall(xsc,1,canvWidth*2,10);
   theWall = createWall(xsc,canvHeight+1,canvWidth*2,10);
   theWall = createWall(1,10,10,canvHeight*2);
   theWall = createWall(canvWidth,10,10,canvHeight*2);

  // connectWebsocket("wss://echo.websocket.events/jimf99/ws");
  // connectWebsocket("ws://localhost:8080/");
  setupJoystick();
  setupButtons();
  startTime = millis();

}

var player1Scan=1;
var player1Auto = 0;
let player1Speed = 0;
let player1Keys = new Array(87, 83, 65, 68, 70);
let thisFrame = 0;
var bKeys = new Array("W", "A", "S", "D", "F");
var mbChr = "x";
var chD = "~";
var player1Ticks = 1000;

var ctr1 = 0;
var ctr2 = 0;
var destHead = 0;

function rotatePlayer1(ang) {
  if (abs(player1.rotation - ang) <= 1) { return; }
  player1.rotation += 1;
}

function scanner() {
  scanner0();
}

function scanner0() {
  var el1 = 1;
  var d1 = 100;
  var a1 = 60;
  el1 = d1/20;
  var startAng=player1.rotation-a1;
  var endAng=player1.rotation+a1;
  var scans = new Array();
  for (ang = startAng; ang < endAng; ang += el1) {
    var px = player1.position.x;
    var py = player1.position.y;
    var ang1 = 90-ang;
    var px1 = px + sin(ang1);
    var py1 = py + cos(ang1);
    for (d = d1*0.60 ; d < d1; d += el1) {
      var px1 = px + sin(ang1) * d;
      var py1 = py + cos(ang1) * d;
      let c = get(px1,py1);
      var pt={x:px1,y:py1,c:c,a:ang1};
      scans.push(pt);
   }
  }
  var newAng=0;
  for(scan of scans) {
    let pt=scan;
    let disp=0;
    let c2=pt.c;
    if(c2[0]>=128) {disp=1;}
    if(c2[1]>128) {disp=0;}
if(disp==1) {
      // fill(c2);
      // rect(pt.x,pt.y,10,10);
      if(newAng==0) {
        player1.rotation=90-pt.a;
        newAng=90-pt.a;
      }
    }
  }
}

function scanner1() {
  var el1 = 10;
  var d1 = 400;
  var a1 = 60;
  var startAng=player1.rotation-a1;
  var endAng=player1.rotation+a1;
  for (ang = startAng; ang < endAng; ang += el1) {
    var px = player1.position.x;
    var py = player1.position.y;
    var ang1 = 90-ang;
    var px1 = px + sin(ang1);
    var py1 = py + cos(ang1);
    for (d = d1/10 ; d < d1; d += el1) {
      var px1 = px + sin(ang1) * d;
      var py1 = py + cos(ang1) * d;
      let c = get(px1,py1);
      let c1=c[0]*256*256+c[1]*256+c[2];
      if(c1>400 && c1<500) {
        text(floor(ang)+' '+floor(d),floor(px1/xsc)*xsc,floor((py1-ysc)/ysc)*ysc);
        if(player1Scan==1) {
          player1.rotation=ang;
          player1Scan=0;
        }
      }
      rect(px1, py1, el1 / 2, el1 / 2);
    }
  }
}

//============================
let button;
var demoButton=0;
let inp;

function setupButtons() {
  let xPos=width-100;
  let yPos=height*0.2;
  yPos-=40;
  button = createButton(' AUTO OFF ');
  button.position(xPos,yPos);
  button.mousePressed(scanOff);
   yPos-=40;
    button = createButton(' AUTO ON ');
    button.position(xPos,yPos);
    button.mousePressed(scanOn);
}

function toggleTrack() {
 drawTrack=!drawTrack;
cans9=!drawTrack;
return true;
}

function scanOn() {
  player1Scan=1;
}

function scanOff() {
  player1Scan=0;
}

function toggleCans() {
  cans9=!cans9;
  drawTrack=!cans9;
  if(cans9==true) {
    newCans();
  }
  return true;    
}

function toggleDemo() {
  player1Auto=1-player1Auto;
  return true;
}

function updateTimer() {
  currentTime = floor((millis() - startTime) / 1000);
  fill(color(255,255,255));
  textSize(32);
  text(convertSeconds(currentTime),width-300,40);
}

// === MAIN DRAW
function draw() {
  frameRate(120);
  background(color(32, 128 , 32));
  if(drawTrack==true) {
   image(track1,0,0,canvWidth,canvHeight);
   image(logo1,20,canvHeight-60,50,50);
  }
  if(drawCones==true) {
    var cs=Cans.size();
    for(i=0; i<cs; i++) {
      var theObj=Cans[i];
      var theX=theObj.position.x-xsc;
      var theY=theObj.position.y-ysc;
      image(cone1,theX,theY,50,50);
    }
  }

  fill(color(255, 255, 0));
  if (player1StartX == 0) {
    player1StartX = player1.position.x;
  }
  if (player1StartY == 0) {
    player1StartY = player1.position.y;
  }
 player1Key = 0;
  player1Key += keyIsDown(player1Keys[0]) * player1Keys[0];
  player1Key += keyIsDown(player1Keys[1]) * player1Keys[1];
  player1Key += keyIsDown(player1Keys[2]) * player1Keys[2];
  player1Key += keyIsDown(player1Keys[3]) * player1Keys[3];
  player1Key += keyIsDown(player1Keys[4]) * player1Keys[4];
  if (recData != null) {
    if (recData.p1k != null) {
      player1Key = recData.p1k;
    }
  }
  for (i = 0; i < bKeys.length; i++) {
    if (mbChr == bKeys[i] && mbChr != "~") {
      player1Key = player1Keys[i];
    }
  }
 player1.distFromBase = dist(player1StartX, player1StartY, player1.position.x, player1.position.y);
  player1.homeBase = player1.distFromBase <= xsc;
  if (player1.hasCan && player1.homeBase == true) {
    player1Key = player1Keys[4];
    rotatePlayer1(player1.rotation + 1);
  }
  if ((player1Auto == 1 && player1Key == 0) && (frameCount % 2 == 0)) { player1Key = player1Keys[0]; }
  if (player1Auto == 1 && player1Key == 0) {
    if (player1StartX * player1StartY > 0) {
      // player1.rotation+=0.15;
      if (dist(player1StartX + xsc, player1StartY + ysc, player1.position.x, player1.position.y) <= xsc && (player1.hasCan == true)) {
        player1Key = player1Keys[4];
        player1Speed = xsc;
      }
    }
  }
  if (player1.homeBase == true) {
    if (player1Key == player1Keys[4] && player1.hasCan == true) {
      player1.hasCan = false;
      player1.score++;
      player1Speed = 0;
      player1.rotation = 180;
    }
  }
 if (player1Key == player1Keys[0]) { // Forward
    player1Speed += 0.1;
  }
  if (player1Key == player1Keys[1]) { // Backward
    player1Speed -= 0.1;
  }
  if (player1Key == player1Keys[2]) { // Right
    player1.rotation -= 5;
  }
  if (player1Key == player1Keys[3]) { // Left
    player1.rotation += 5;
  }
  if (player1Speed > 5) { player1Speed = 5; }
  if (player1Speed < -5) { player1Speed = -5; }
  if (player1Speed < -xsc * 2) { player1Speed = -xsc * 2; }
  if (player1Speed > xsc * 2) { player1Speed = xsc * 2; }
  player1.setSpeed(player1Speed, player1.rotation);
  player1.collide(walls);
 if (frameCount % 2 == 0) {
    player1.collide(Cans);
  }
 player1.overlap(Cans, tankHitCan);
  player1.age++;
 if (1 == 1) {
    strokeWeight(2);
    stroke(color(255, 255, 255));
    fill(player1.shapeColor);
    // rect(player1StartX - xsc, player1StartY - xsc, 40, 40);
    if (player1Auto == 0) {
      strokeWeight(2);
      stroke(color(255, 255, 255));
    }
  }
drawSprites();
  if(Cans.size()>0 && cans9==false && drawTrack==true) {
    Cans[0].remove();
  }

drawJoystick();
  if(player1Scan==1) {scanner();}
 if (player1Auto == 1) {
    if (player1.hasCan == false && Cans.length > 0) {
      var i = (Cans.length - 1);
      var d = {};
      d.y = objs[i].y - player1.position.y;
      d.x = objs[i].x - player1.position.x;
      var angle1 = atan2(d.y, d.x);
      player1.rotation = angle1;
    }
  }
}

function getTank(x, y, tankColor) {
  let tank = createSprite(x, y, xsc * 2, ysc * 2);
  tank.shapeColor = tankColor;
  tank.age = 0;
  this.drawSelf = false;
  tank.score = 0;
  tank.hasCan = false;
  tank.isInvincible = function() {
    return false;
    return tank.age <= 180;
  }

  tank.draw = function() {
    if (this.age <= 0) { return true; }
    this.drawSelf = true;
    text(this.x, 100, 100);
    if (this.drawSelf) {
      image(car1,0,0,xsc*3.5,ysc*3.5);
     rectMode(CORNER);
      // fill(tankColor);
      // strokeWeight(2);
      // stroke("black");
      // rect(-xsc, -ysc, xsc * 1.25, ysc / 2);
      // rect(-xsc, ysc / 2, xsc * 1.25, ysc / 2);
      // rect(-xsc / 2, -ysc / 2, xsc / 2, ysc);
      // rect(0, -ysc / 5, xsc, ysc / 2);
      if (this.hasCan == true) {
        fill(color(255, 165, 0));
        ellipse(0, 0, 20, 20);
      }
    }
  }

  tank.shoot = function() {
    return true;
  }
  tank.setDefaultCollider();
  return tank;
}

function createWall(x, y, wallWidth, wallHeight) {
  wall = createSprite(x + xoff, y + yoff, wallWidth, wallHeight);
  wall.shapeColor = color(249, 195, 89);
  wall.immovable = false;
  wall.setDefaultCollider();
  walls.add(wall);
  return wall;
}

function keyPressed() {
  return true;
}

function tankHitCan(tank, Can) {
  if (tank.shapeColor != Can.shapeColor) {
    tank.age = 0;
    var thePlayer = player1;
    if (tank == thePlayer && tank.hasCan == false) {
      Can.remove();
      thePlayer.hasCan = true;
      if (thePlayer == player1 && player1StartX * player1StartY > 0) {
        var d = {};
        d.y = player1StartY - player1.position.y;
        d.x = player1StartX - player1.position.x;
        var angle1 = atan2(d.y, d.x);
        if (player1Auto == 1) {
          thePlayer.rotation = angle1;
          thePlayer.speed = xsc * 2;
        }
      }
    }
  }
}

// === JoyStick
let centX, centY;
let th;
let p180;
function setupJoystick() {
  centX = width / 2;
  centY = height / 2;
  background(0);
  th = 0;
  p180 = PI / 180;
}
function drawJoystick() {
  strokeWeight(1);
  stroke(255, 255, 255);
  noFill();
  let joyLen = 100;
  let joyLen2 =floor(joyLen/4);
  let joyX1 = floor(width * 0.80);
  let joyY1 = floor(height * 0.10);
  if (jmouseX1 == 0 || jmouseY1 == 0) {
    jmouseX1 = joyX1;
    jmouseY1 = joyY1;
  }
  line(joyX1, joyY1, joyX1, joyY1 - joyLen2 / 2);
  line(joyX1, joyY1, joyX1, joyY1 + joyLen2 / 2);
  line(joyX1 - joyLen2 / 2, joyY1, joyX1 + joyLen2 / 2, joyY1);
  line(joyX1 - joyLen2 / 2, joyY1, joyX1 + joyLen2 / 2, joyY1);
  circle(joyX1, joyY1, joyLen);
  if (mouseIsPressed == true && dist(mouseX, mouseY, joyX1, joyY1) <= joyLen * 0.60) {
    th = -1 * (mouseX - joyX1) / (joyLen / 1.8);
    th = constrain(th * 100, -90, 90);
    thY = -1 * (mouseY - joyY1) / (joyLen / 1.8);
    thY = constrain(thY * 100, -90, 90);
    jmouseX1 = mouseX;
    jmouseY1 = mouseY;
    head1 = -1 * th;
  }
  if (mouseIsPressed == false) {
    jmouseX1 = joyX1 + sin(player1.rotation * p180) * (joyLen) * (player1Speed / 10);
    jmouseY1 = joyY1 - cos(player1.rotation * p180) * (joyLen) * (player1Speed / 10);
  }
  strokeWeight(8);
  circle(jmouseX1, jmouseY1, 10);
  line(joyX1, joyY1, jmouseX1, jmouseY1);
  strokeWeight(1);
  text(" JOYSTICK ", joyX1 - 8, joyY1 + 64);
  text(" KEYS: W A S D ", joyX1 - 8, joyY1 + 80);
  strokeWeight(1);
  stroke(255, 255, 255);
  noFill();
  var vel1 = dist(joyX1, joyY1, jmouseX1, jmouseY1) / joyLen * 2;
  vel1 = constrain(vel1, 0, 1);
  if (jmouseY1 >= joyY1) { vel1 = -1 * vel1; }
  let th1 = th;
     if (mouseIsPressed == true && dist(mouseX, mouseY, joyX1, joyY1) <= joyLen * 0.60) {
   player1.rotation += (-th1 / 10);
    player1Speed = vel1 * 3.0;
  }
}


function mousePressed() {
}

function messageReceived(data) {
  recData = data;
}
