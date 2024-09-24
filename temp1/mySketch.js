/******************
Code inspired by Vamoss
Source: https://openprocessing.org/sketch/1440432
github: http://github.com/vamoss
******************/
var client;
var topic="jimfyyc/mqtt/p5js";
var broker="wss://test.mosquitto.org:8081";
var prevX=0;
var prevY=0;
function setup() {
	createCanvas(windowWidth, windowHeight);
	background(255);
	client = mqtt.connect(broker)
  client.subscribe(topic, function (err) {
		console.log("mqtt subscribed");
		if(err) console.error(err);
  })
	
	client.on('connect', function (connack) {
		console.log("mqtt connected", connack.sessionPresent)
	})
	
	client.on("message", function (topic, payload) {
		console.log(topic+" "+payload.toString())
		var message = JSON.parse(payload.toString())
		var x = Math.floor(message.x * width)
		var y = Math.floor(message.y * height)
		var size = Math.floor(message.size * width)
		fill(message.r * 255,
				 message.g * 255,
				 message.b * 255)
		if(message.shape == "circle"){
			circle(x, y, size)
		}else if(message.shape == "rect"){
			rect(x, y, size, size)
		}else if(message.shape == "triangle"){
			beginShape()
			for(var a = 0; a < TWO_PI; a += TWO_PI/3){
				vertex(x + cos(a) * size, y + sin(a) * size)
			}
			endShape()
		};
		txt=message.txt+"";
		if(txt>char(0) && txt>char(32) ) {
			textSize(32);
			fill(color(0,0,0));
			text(txt,x+5,y+5);
		};
  });
}

var ky=0;

function draw() {
	if(ky==32) {background(255);}
	var plotX=round(mouseX/width,4);
	var plotY=round(mouseY/height,4);
	if(mouseIsPressed){
		if(plotX != prevX && plotY !=prevY) {
		  sendShape(plotX,plotY,0.01);
		};
		prevX=plotX;
		prevY=plotY;
	}
	ky=0;
}

function keyPressed(){
	ky=keyCode;
	sendShape(random(), random(), 0.05);
}

var prevMsg="";

function sendShape(x, y, size, ob){
	x=round(x,4);
	y=round(y,4);
	size=round(size,4);
	var shps=["circle", "rect", "triangle"];
  var shp=random(shps);
	ch=char(ky);
	if(ch=='W' || ch=='w') {shp=shps[0];}
	if(ch=='A' || ch=='a') {shp=shps[1];}
	if(ch=='S' || ch=='s') {shp=shps[2];}
	if(ch=='D' || ch=='d') {shp=shps[0];}
	var message = {
		ts:Date.now(),
		txt: ch+"",
		shape: shp,
		x, y, size,
		r: round(random(),4),
		g: round(random(),4),
		b: round(random(),4)
	}
	client.publish(topic, JSON.stringify(message)) ; 
}

