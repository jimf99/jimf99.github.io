let faces =[]

let img;
	let mX=0;
	let mY=0;
	let ctr=0;

function putMsg(msg1,val1) {
		document.getElementById(msg1).innerHTML = val1;
}

function setup() {
  createCanvas(1000,480);
  var i=0;
  var y=0;
  faces.push(new Face(200,140));
  img = loadImage('image.png');
}

let turn="";

let xPlays="";
let oPlays="";
let playUsed="";

function sortPlays(thePlays) {
	var tmp=thePlays.split('');
	tmp.sort();
	var tmp1="";
	for(i=0; i<tmp.length; i++) {
		tmp1+=tmp[i];
	}
	return tmp1;
}

function checkSeq(thePlay,theSeq) {
	return
}

function checkWins(playerTurn) {
	  if(playerTurn=='X') {thePlays=xPlays;}
	  if(playerTurn=='O') {thePlays=oPlays;}
	  alert(playerTurn+" "+thePlays);
	  return;
}

function doTurn(thePos) {
	if(thePos>"") {
		if(playUsed.indexOf(thePos)>-1) {return;}
		playUsed+=thePos;
	}
    if(turn=="X") {xPlays+=thePos;}
    if(turn=="O") {oPlays+=thePos;}
	xPlays=sortPlays(xPlays);
	oPlays=sortPlays(oPlays);
	if(turn=="X") {turn="O";return;}
	if(turn=="O") {turn="X";return;}
}

function resetGame() {
  xPlays="";oPlays="";turn="X";
  playUsed="";
  drawGame();
}

function drawGame() {
	let h1=100;
	let w1=100;
	let ctr=0;
	let xOffset=400;
	let yOffset=60;
	fill("#ffffff");
	strokeWeight(1);
	for(yy=0;yy<3; yy++) {
		let yPos=yOffset+(yy*h1);
		for(xx=0; xx<3; xx++) {
			let xPos=xOffset+(xx*w1);
			fill("#ffffff");
			rect(xPos,yPos,w1,h1);
			fill("#000000");
			textSize(96);
			ctr++;
			theText=str(ctr);
			if(xPlays.indexOf(theText)>-1) {theText="X";}
			if(oPlays.indexOf(theText)>-1) {theText="O";}
			text(theText,xPos+(w1*0.2),yPos+(h1*0.8));
		}
	}
	textSize(18);
	var mx=-1;
	var my=-1;
	if(clickX>=xOffset && clickY>=yOffset) {
	  mx=int((clickX-xOffset)/h1);
	  my=int((clickY-yOffset)/w1);
	}
	if(mx>2) {mx=-1;}
	if(my>2) {my=-1;}
	if(mx>=0 && my>=0) {
	  strokeWeight(1);
	  text(str(mx)+","+str(my),mouseX-20,mouseY-10);
	  if(mx==0 && my==0) {doTurn(1);playAudio('1');}
	  if(mx==1 && my==0) {doTurn(2);playAudio('2');}
	  if(mx==2 && my==0) {doTurn(3);playAudio('3');}
	  if(mx==0 && my==1) {doTurn(4);playAudio('4');}
	  if(mx==1 && my==1) {doTurn(5);playAudio('5');}
	  if(mx==2 && my==1) {doTurn(6);playAudio('6');}
	  if(mx==0 && my==2) {doTurn(7);playAudio('7');}
	  if(mx==1 && my==2) {doTurn(8);playAudio('8');}
	  if(mx==2 && my==2) {doTurn(9);playAudio('9');}
	  putMsg('msg1',str(mx)+","+str(my));
	  putMsg('msg1','x='+xPlays+' o='+oPlays+" "+playUsed);
	}
	value=0;
	clickX=0;
	clickY=0;
	if (turn=="") {turn="X";}
	
}

let value=0;
let clickX=0;
let clickY=0;

function checkClicked(posx,posy,clickX,clickY,r1,msg) {
	var result="";
	let d1=dist(posx,posy,clickX,clickY);
	if(result=="" && d1<r1) {result=msg;}
	return result;
}

function mouseClicked() {
	value=255-value;
	clickX=int(mouseX);
	clickY=int(mouseY);
	let d="";
	if(d=="") {d=checkClicked(152,142,clickX,clickY,30,'LeftEye');}
    if(d=="") {d=checkClicked(263,143,clickX,clickY,30,'RightEye');}
	if(d=="") {d=checkClicked(207,199,clickX,clickY,30,'Nose');}
    if(d=="") {d=checkClicked(205,274,clickX,clickY,30,'Mouth');}
	if(d=='Mouth') {playSaying('1');resetGame();}
	putMsg('msg1',str(clickX)+","+str(clickY)+","+d);
}

function draw() {
	drawGame();
	faces.forEach(face => {
		face.draw();
	})
	fill("#550000")
	rect(170,210, 80,20);
	fill("#550000")
	rect(155,264, 120,20);
	image(img, 80, 20, img.width/2,img.height/2);
	ctr=ctr+1;
	if(ctr>80) {
		ctr=0;
		mX=random(width*2);
		mY=random(height*2);
	}
}

function Face(x,y){
  var sc1=15;
  this.x = x-sc1*3.2;
  this.y = y+5;
  this.blinkPause = random(100,400)
  var leftX=this.x;
  this.leftEye = new Eye(leftX, this.y,this.blinkPause)
  this.rightEye = new Eye(leftX+sc1*7.6,this.y,this.blinkPause)
  this.draw = function(){
    this.rightEye.draw()
    this.rightEye.blink()
    this.leftEye.draw()
    this.leftEye.blink()
  }
}

function randomInt(max) {
  return Math.floor(Math.random() * max);
}

function Eye(x,y,binkPause){
  this.x = x;
  this.y = y;
  this.d = 30 // diameter of circle
  this.topLidY = this.y
  this.dy = 1
  this.distance = 0,
  this.angle = 0
  this.blinkPause = 0;// duration till next bink
  this.topLidYOrigin = this.y // original position before animation
  this.bottomLidY = this.y - this.d;
  this.blink = function() {
    // decrement blink pause duration
    if(this.blinkPause > 0){
      this.blinkPause -= 1
      // return function to exit function early
      return
    }
    if(this.topLidY >= this.topLidYOrigin + this.d /2 ){
      this.blinkPause = binkPause
      this.dy = -this.dy
    }else if(this.topLidY < this.topLidYOrigin){
      this.dy = -this.dy
    }
    
    // animate eyelids
    this.topLidY += this.dy
    this.bottomLidY -= this.dy;
	
  },
  this.draw = function(){
    // eye ball
    noStroke()
    fill("white")
    circle(this.x,this.y, this.d)
    
    // pupil
    push();
    fill("black")
    // distance from mouse to eyeball center
    this.distance = constrain(int(dist(this.x,this.y,mX,mY)), 0, height)
    // map distance value from mouse position over eyeball radius
    this.eyePos = map(this.d /3 , 0,500,0,this.distance )
    this.angle = atan2(mY - this.y, mX - this.x);
    translate(this.x, this.y);
    rotate(this.angle);
    // circle( distance from eye center, offset from angle, circe diameter)
    circle(this.eyePos, 0, this.d / 3);
    pop();
    // eye lids
    fill("#550000")
      rect(this.x-this.d, this.topLidY,  this.d*2, this.d)
	if(this.bottomLidY>20) {
      rect(this.x-this.d, this.bottomLidY,  this.d*2, this.d)
	}
    // eyeliner
    noFill()
    strokeWeight(3)
    stroke("black")
    circle(this.x,this.y, this.d);
  }

}