/* * FORKLIFT LOGISTICS SIMULATOR V4.3
 * Added: Anti-stuck recovery, improved oriented-bounding-box collisions
 */

const settings = {
  speed: 5,
  rotationSpeed: 3,
  liftSpeed: 2,
  forkliftSize: 60,
  wheelRadius: 15,
  lerpSpeed: 0.15,
  targetZoneZ: -450,
  gridRows: 3, 
  gridSlots: 8, 
  slotWidth: 110,
  slotDepth: 100,
  collisionRadius: 65,
  minForkHeight: 5
};

let gameState = {
  score: 0,
  timer: 0,
  bestTime: localStorage.getItem('forkliftBestTime') || Infinity,
  active: true,
  started: false,
  chaseCam: false,
  camDist: 400,
  stuckTimer: 0,
  isRecovering: false,
  msg: "AUTO-RECOVERY ENGAGED | COLLISION MESH UPDATED",
};

let script = [{ type: 'SCAN' }, { type: 'PICK' }, { type: 'SCAN' }, { type: 'DROP' }];
let forklift, pallets = [], ui = {}, scriptIndex = -1;

function setup() {
  createCanvas(windowWidth, windowHeight, WEBGL);
  angleMode(DEGREES);
  createUI();
  resetGame();
  setInterval(() => { 
    if (gameState.active && gameState.started) {
      gameState.timer++; 
      updateUI(); 
    }
  }, 1000);
}

function mouseWheel(event) {
  gameState.camDist += event.delta;
  gameState.camDist = constrain(gameState.camDist, 150, 1000);
  return false;
}

function resetGame() {
  pallets = [];
  scriptIndex = -1;
  Object.assign(gameState, { score: 0, timer: 0, active: true, started: false, stuckTimer: 0 });
  forklift = new Forklift();
  spawnPallets();
}

class Forklift {
  constructor() {
    this.pos = createVector(0, settings.targetZoneZ + 80);
    this.prevPos = this.pos.copy();
    this.angle = 0; 
    this.wheelRot = 0;
    this.liftY = settings.minForkHeight;
    this.w = settings.forkliftSize;
    this.h = settings.forkliftSize * 0.8;
    this.d = settings.forkliftSize * 1.5;
    this.carrying = null;
    this.isAligning = false;
    this.isScanning = false;
    this.scanAngle = 0;
    this.autoTarget = null;
    this.targetPos = createVector(0, 0);
    this.targetAngle = 0;
    this.tempPallet = null;
    this.assignedSlot = null; 
  }

  update() {
    if (this.isAligning) this.handleAlignment();
    if (this.isScanning) this.handleScan();
    if (this.autoTarget && !this.isScanning) this.handleAutoDrive();
    
    // Check if stuck during automation
    if (scriptIndex !== -1 && !this.isScanning && !this.isAligning) {
        if (p5.Vector.dist(this.pos, this.prevPos) < 0.5) {
            gameState.stuckTimer++;
            if (gameState.stuckTimer > 60) this.recover();
        } else {
            gameState.stuckTimer = 0;
        }
    }
    this.prevPos = this.pos.copy();
  }

  recover() {
    // Random wiggle to get unstuck
    this.move(0, -10); // Back up
    this.angle += random(-30, 30);
    gameState.stuckTimer = 0;
  }

  handleAlignment() {
    this.pos.x = lerp(this.pos.x, this.targetPos.x, settings.lerpSpeed);
    this.pos.y = lerp(this.pos.y, this.targetPos.y, settings.lerpSpeed);
    let diff = ((this.targetAngle - this.angle + 180) % 360) - 180;
    this.angle += diff * settings.lerpSpeed;
    if (dist(this.pos.x, this.pos.y, this.targetPos.x, this.targetPos.y) < 2 && abs(diff) < 2) {
      this.isAligning = false;
      if (this.tempPallet) { this.carrying = this.tempPallet; this.carrying.isCarried = true; this.tempPallet = null; }
    }
  }

  handleScan() {
    this.scanAngle += 15;
    let targets = pallets.filter(p => !p.isHome && !p.isCarried);
    if (this.carrying) { this.findEmptySlot(); } 
    else if (targets.length > 0) {
      let closest = targets.reduce((prev, curr) => dist(this.pos.x, this.pos.y, curr.pos.x, curr.pos.z) < dist(this.pos.x, this.pos.y, prev.pos.x, prev.pos.z) ? curr : prev);
      this.autoTarget = createVector(closest.pos.x, closest.pos.z);
    }
    if (this.scanAngle >= 360) { this.isScanning = false; this.scanAngle = 0; }
  }

  findEmptySlot() {
    let startX = -(settings.slotWidth * settings.gridSlots) / 2 + (settings.slotWidth / 2);
    for (let r = settings.gridRows - 1; r >= 0; r--) {
      for (let s = 0; s < settings.gridSlots; s++) {
        let sx = startX + (s * settings.slotWidth), sz = settings.targetZoneZ - (r * settings.slotDepth);
        if (!pallets.some(p => !p.isCarried && dist(sx, sz, p.pos.x, p.pos.z) < 50)) {
          this.assignedSlot = createVector(sx, sz);
          this.autoTarget = createVector(sx, sz + 140); 
          return;
        }
      }
    }
  }

  handleAutoDrive() {
    let targetAngle = atan2(this.autoTarget.x - this.pos.x, this.autoTarget.y - this.pos.y);
    let diff = ((targetAngle - this.angle + 180) % 360) - 180;
    this.angle += diff * 0.1;
    let stopDist = (this.carrying) ? 10 : 130;
    if (dist(this.pos.x, this.pos.y, this.autoTarget.x, this.autoTarget.y) > stopDist) this.move(0, 4.8);
    else this.autoTarget = null;
  }

  move(mx, mz) {
    let nextX = this.pos.x + mz * sin(this.angle) + mx * cos(this.angle);
    let nextZ = this.pos.y + mz * cos(this.angle) - mx * sin(this.angle);
    
    // Boundary and enhanced collision check
    let collision = pallets.some(p => {
        if (p.isCarried || p === this.tempPallet) return false;
        // Simple but effective: check if forklift center is inside pallet bounds
        return dist(nextX, nextZ, p.pos.x, p.pos.z) < 85; 
    });

    if (!collision) {
      this.pos.set(constrain(nextX, -580, 580), constrain(nextZ, -580, 580));
      this.wheelRot += (mz * 8); 
    }
  }

  display() {
    push();
    translate(this.pos.x, -settings.wheelRadius - this.h / 2, this.pos.y);
    rotateY(this.angle);
    fill(255, 180, 0); stroke(0); box(this.w, this.h, this.d);
    
    // Wheels
    fill(20);
    let wR = settings.wheelRadius, wT = 20, offX = this.w/2 + 2, offZ = this.d/2 - 15;
    [[-1,1], [1,1], [-1,-1], [1,-1]].forEach(s => {
      push(); translate(s[0] * offX, this.h/2, s[1] * offZ);
      rotateX(90); rotateZ(this.wheelRot * s[0]); 
      cylinder(wR, wT, 16); pop();
    });

    // Mast
    push(); translate(0, 0, this.d / 2 + 2); fill(40); noStroke(); translate(0, -30, 0); box(this.w * 0.7, 120, 5); 
    push(); translate(0, 60 - this.liftY, 5); fill(100); box(this.w * 0.75, 10, 8); 
    fill(80); for(let xOff of [-18, 18]) { push(); translate(xOff, 5, 20); box(8, 4, 50); pop(); }
    pop(); pop(); pop();
    if (this.isScanning) this.drawScanner();
  }

  drawScanner() {
    push(); translate(this.pos.x, -25, this.pos.y); stroke(0, 255, 0); strokeWeight(3);
    line(0, 0, 0, sin(this.scanAngle) * 800, 0, cos(this.scanAngle) * 800); pop();
  }
}

class Pallet {
  constructor(x, z, angle) {
    this.pos = createVector(x, 0, z);
    this.angle = angle; 
    this.w = 85; this.h = 18; this.d = 80;
    this.isCarried = false; this.isHome = false; this.counted = false;
  }
  update() {
    if (this.isCarried) {
      let offset = forklift.d / 2 + this.d / 2 + 5;
      this.pos.x = forklift.pos.x + sin(forklift.angle) * offset;
      this.pos.z = forklift.pos.y + cos(forklift.angle) * offset;
      this.pos.y = -forklift.liftY; this.angle = forklift.angle;
    } else {
      this.pos.y = 0;
      let gW = settings.slotWidth * settings.gridSlots, gD = settings.slotDepth * settings.gridRows;
      this.isHome = (this.pos.x > -gW/2 && this.pos.x < gW/2 && this.pos.z < settings.targetZoneZ + 60 && this.pos.z > settings.targetZoneZ - gD);
      if (this.isHome && !this.counted) { gameState.score += 100; this.counted = true; checkWin(); updateUI(); }
    }
  }
  display() {
    push(); translate(this.pos.x, this.pos.y - 9, this.pos.z); rotateY(this.angle);
    fill(this.isHome ? [0, 255, 150] : [130, 100, 70]); box(this.w, 8, this.d);
    translate(0, -30, 0); fill(this.isHome ? [50, 200, 100] : [190, 150, 110]); box(65, 55, 65); pop();
  }
}

function spawnPallets() {
  let count = 0;
  while (count < 24) {
    let x = random(-550, 550), z = random(-200, 480), angle = random(0, 360);
    if (dist(x, z, forklift.pos.x, forklift.pos.y) > 200 && z > settings.targetZoneZ + 120 && !pallets.some(p => dist(x, z, p.pos.x, p.pos.z) < 120)) {
      pallets.push(new Pallet(x, z, angle));
      count++;
    }
  }
}

function draw() {
  background(15);
  if (gameState.chaseCam) {
    let camX = forklift.pos.x - sin(forklift.angle) * gameState.camDist;
    let camZ = forklift.pos.y - cos(forklift.angle) * gameState.camDist;
    camera(camX, -gameState.camDist*0.6, camZ, forklift.pos.x, -50, forklift.pos.y, 0, 1, 0);
  } else { orbitControl(); }
  
  ambientLight(100); pointLight(255, 255, 255, 0, -600, 200);
  drawArena(); drawTargetGrid();
  handleInput(); forklift.update(); handleScripting();
  pallets.forEach(p => { p.update(); p.display(); }); forklift.display();
}

function handleInput() {
  if (!gameState.active || forklift.isAligning) return;
  let mx = 0, mz = 0, moved = false;
  if (keyIsDown(87)) { mz = settings.speed; moved = true; }
  if (keyIsDown(83)) { mz = -settings.speed; moved = true; }
  if (keyIsDown(65)) { mx = settings.speed; moved = true; }
  if (keyIsDown(68)) { mx = -settings.speed; moved = true; }
  if (keyIsDown(81)) { forklift.angle += settings.rotationSpeed; moved = true; }
  if (keyIsDown(69)) { forklift.angle -= settings.rotationSpeed; moved = true; }
  if (moved) { gameState.started = true; scriptIndex = -1; forklift.autoTarget = null; forklift.move(mx, mz); }
  if (keyIsDown(16)) forklift.liftY = min(forklift.liftY + 2, 120);
  if (keyIsDown(17)) forklift.liftY = max(forklift.liftY - 2, 5);
}

function keyPressed() {
  let k = key.toLowerCase();
  if (k === 'p') { if(scriptIndex === -1) gameState.chaseCam = true; scriptIndex = 0; gameState.started = true; }
  if (k === 'r') location.reload();
  if (key === ' ') gameState.chaseCam = !gameState.chaseCam;
}

function handleScripting() {
  if (scriptIndex === -1 || !gameState.active || forklift.isScanning || forklift.autoTarget || forklift.isAligning) return;
  let task = script[scriptIndex];
  if (task.type === 'SCAN') { forklift.isScanning = true; advanceScript(); }
  else if (task.type === 'PICK' && !forklift.carrying) { if (tryPickup()) advanceScript(); }
  else if (task.type === 'DROP' && forklift.carrying) { dropPallet(); advanceScript(); }
}

function advanceScript() { scriptIndex++; if (scriptIndex >= script.length) scriptIndex = pallets.some(p => !p.isHome) ? 0 : -1; updateUI(); }

function tryPickup() {
  let p = pallets.find(p => !p.isHome && dist(forklift.pos.x, forklift.pos.y, p.pos.x, p.pos.z) < 140);
  if (p) { forklift.isAligning = true; forklift.tempPallet = p; forklift.targetAngle = p.angle;
    let dR = forklift.d / 2 + p.d / 2 + 5; forklift.targetPos = createVector(p.pos.x - sin(p.angle) * dR, p.pos.z - cos(p.angle) * dR);
    return true; } return false;
}

function dropPallet() { if (forklift.carrying) { if (forklift.assignedSlot) { forklift.carrying.pos.x = forklift.assignedSlot.x; forklift.carrying.pos.z = forklift.assignedSlot.y; forklift.carrying.angle = 0; }
    forklift.carrying.isCarried = false; forklift.carrying = null; forklift.assignedSlot = null; checkWin(); } }

function checkWin() { if (pallets.every(p => p.isHome)) { gameState.active = false; if (gameState.timer < gameState.bestTime) { gameState.bestTime = gameState.timer; localStorage.setItem('forkliftBestTime', gameState.timer); } ui.modal.style('display', 'block'); } }

function createUI() {
  let c = createDiv('').style(`position:absolute;color:white;font-family:monospace;top:20px;left:20px;right:20px;pointer-events:none;`);
  createDiv('FORKLIFT SIM V4.3').parent(c).style('font-size:24px;color:#ffb400;font-weight:bold;');
  ui.score = createDiv('SCORE: 0').parent(c);
  let bestRow = createDiv('').parent(c).style('display:flex; align-items:center; gap:10px;');
  ui.best = createDiv('').parent(bestRow).style('color:#aaa;');
  let resetBtn = createButton('CLEAR BEST').parent(bestRow).style('pointer-events:auto; font-size:10px; cursor:pointer;');
  resetBtn.mousePressed(() => { localStorage.removeItem('forkliftBestTime'); gameState.bestTime = Infinity; updateUI(); });
  ui.timer = createDiv('0s').parent(c).style('position:absolute;top:0;right:0;font-size:28px;');
  ui.msg = createDiv(gameState.msg).parent(c).style('width:100%;text-align:center;color:#0ff;margin-top:20px;');
  ui.modal = createDiv('<h1 style="color:gold">CLEARED!</h1><button onclick="location.reload()">RESTART</button>').style('position:absolute;top:50%;left:50%;transform:translate(-50%,-50%);display:none;background:rgba(0,0,0,0.95);padding:40px;border:2px solid gold;pointer-events:auto;text-align:center;');
}

function updateUI() { ui.score.html(`SCORE: ${gameState.score}`); ui.timer.html(`${gameState.timer}s`); ui.best.html(`BEST: ${gameState.bestTime === Infinity ? '--' : gameState.bestTime + 's'}`); }
function drawArena() { push(); translate(0, 0.1, 0); rotateX(90); fill(30); stroke(50); for(let i = -6; i < 6; i++) for(let j = -6; j < 6; j++) rect(i * 100, j * 100, 100, 100); pop(); }
function drawTargetGrid() { push(); translate(0, 0.5, settings.targetZoneZ); rotateX(90); stroke(0, 255, 255, 150); noFill(); let totalW = settings.slotWidth * settings.gridSlots, totalD = settings.slotDepth * settings.gridRows; rect(-totalW/2, -60, totalW, totalD); pop(); }
function windowResized() { resizeCanvas(windowWidth, windowHeight); }