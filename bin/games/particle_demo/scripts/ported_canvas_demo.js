
RequireSystemScript("colors.js");

var KL = KL || {};

KL.Particle = function(x, y, z, gV) {

	this.v = new KL.V3(x, y, z);
	this.oldV = new KL.V3(x, y, z);
	this.gV = gV || null;
	this.velocity = new KL.V3();
	this.length = 0;

	this.move = function () {
		var dx = this.gV.x - this.v.x;
		var dy = this.gV.y - this.v.y;
		var dz = this.gV.z - this.v.z;
		var d = Math.sqrt( dx*dx + dy*dy + dz*dz );
		if (d<0.5) {
			var rnd = Math.random()*3;
			dx = this.velocity.x + rnd*dx/d;
			dy = this.velocity.y + rnd*dy/d;
			dz = this.velocity.z + rnd*dz/d;
		}
		else {
			dx = this.velocity.x + 0.2*dx/d;
			dy = this.velocity.y + 0.2*dy/d;
			dz = this.velocity.z + 0.2*dz/d;
		}
		this.velocity.set(dx, dy, dz);
		this.velocity.multiply(0.975);



		this.oldV.x = this.v.x;
		this.oldV.y = this.v.y;
		this.oldV.z = this.v.z;
		this.oldV.px = this.v.px;
		this.oldV.py = this.v.py;
		this.v.add(this.velocity);
	};

	this.toString = function () {
		return 'KL.Particle ( ' + this.v.x + ', ' + this.v.y + ', ' + this.v.z + ' )';
	};
}

KL.V3 = function ( x, y, z ) {

	this.x = x || 0;
	this.y = y || 0;
	this.z = z || 0;

	this.px = 0;
	this.py = 0;

	this.set = function ( x, y, z ) {
		this.x = x;
		this.y = y;
		this.z = z;

		return this;
	};

	this.add = function ( v ) {
		this.x+= v.x;
		this.y+= v.y;
		this.z+= v.z;
	};

	this.multiply = function ( n ) {
		this.x*= n;
		this.y*= n;
		this.z*= n;
	};

	this.divide = function ( n ) {
		this.x/= n;
		this.y/= n;
		this.z/= n;
	};

	this.length = function () {
		return Math.sqrt( this.x*this.x + this.y*this.y + this.z*this.z );
	};

	this.toString = function () {
		return 'KL.V3 ( ' + this.x + ', ' + this.y + ', ' + this.z + ' )';
	};
};

KL.M3 = function ( n11, n12, n13, n21, n22, n23, n31, n32, n33 ) {

	this.n11 = n11 || 1; this.n12 = n12 || 0; this.n13 = n13 || 0;
	this.n21 = n21 || 0; this.n22 = n22 || 1; this.n23 = n23 || 0;
	this.n31 = n31 || 0; this.n32 = n32 || 0; this.n33 = n33 || 1;

	this.rot = function (rx, ry, rz) {
		var cosx=Math.cos(rx), sinx=Math.sin(rx);
		var cosy=Math.cos(ry), siny=Math.sin(ry);
		var cosz=Math.cos(rz), sinz=Math.sin(rz);

		this.n11 = cosz*cosy;
		this.n12 = sinz*cosy;
		this.n13 = -siny;

		this.n21 = cosz*siny*sinx - sinz*cosx;
		this.n22 = sinz*siny*sinx + cosx*cosz;
		this.n23 = sinx*cosy;

		this.n31 = cosz*siny*cosx + sinz*sinx;
		this.n32 = sinz*siny*cosx - cosz*sinx;
		this.n33 = cosx*cosy;
	};

	this.xV3 = function ( v ) {

		var vx = v.x,
			vy = v.y,
			vz = v.z;

		v.x = this.n11 * vx + this.n12 * vy + this.n13 * vz;
		v.y = this.n21 * vx + this.n22 * vy + this.n23 * vz;
		v.z = this.n31 * vx + this.n32 * vy + this.n33 * vz;

		return v;
	};

	this.xParticle = function ( p ) {

		var v = p.v,
			oldV = p.oldV;
		oldV.x = v.x;
		oldV.y = v.y;
		oldV.z = v.z;
		oldV.px = v.px;
		oldV.py = v.py;

		this.xV3(v)

		return p;
	};
};

function minmax(x, min, max) {
	if (x<min) return min;
	else if (x>max) return max;
	else return x;
}

const NUM_BRANCHES = 32;
const NUM_PER_BRANCH = 16;
const NUM_PARTICLES = NUM_BRANCHES*NUM_PER_BRANCH;
const MAX = 20;

const SCREEN_WIDTH = GetScreenWidth();
const SCREEN_HEIGHT = GetScreenHeight();
const SCREEN_OX = SCREEN_WIDTH/2;
const SCREEN_OY = SCREEN_HEIGHT/2;

function ParticleDemo(){

	this.particles = [];
	this.rotMatrix = new KL.M3();
	this.rotX = 0;
	this.rotY = 0;
	this.rotZ = 0;
	this.cnt = 0;
	this.gV = new KL.V3();

	this.speedX = 0.05 * Math.PI / SCREEN_WIDTH;
    this.speedY = 0.05 * Math.PI / SCREEN_HEIGHT;

    this.shader = GetDefaultShaderProgram();
    this.image = new Image(1, 1, new Color(0xFF, 0xFF, 0xFF));

    this.initParticles = function() {


		// random
        for (var i=0; i<NUM_PARTICLES; i++) {
            let x = Math.random()*MAX-MAX/2,
                y = Math.random()*MAX-MAX/2,
                z = Math.random()*MAX-MAX/2;
            let lColor = new Color(Math.floor(Math.random()*255), Math.floor(Math.random()*255), Math.floor(Math.random()*255));
            let particle = new KL.Particle(x, y, z, this.gV);
            let shape = new Shape([{x:SCREEN_OX, y:SCREEN_OY, color:lColor}, {x:SCREEN_OX+2, y:SCREEN_OY, color:lColor}, {x:SCREEN_OX+2, y:SCREEN_OY+2, color:lColor}, {x:SCREEN_OX, y:SCREEN_OY+2, color:lColor}], this.image);
            particle.Group = new Group([shape], this.shader);
            particle.OldSide = 0;
            this.particles.push(particle);
        }

		// init px, py for oldV
		this.particles.forEach(function(particle){
			var zoom = 600/(50+particle.v.z);
			var px = (particle.v.x*zoom);
			var py = (particle.v.y*zoom);
			particle.v.px = px;
			particle.v.py = py;
		});

	}

	//
    this.loop = function() {

		if (IsMouseButtonPressed(MOUSE_LEFT)) {
            this.rotX = minmax((GetMouseX())*this.speedY, -0.05, 0.05);
            this.rotY = minmax((-GetMouseY())*this.speedX, -0.05, 0.05);
            this.rotZ = 0;
			this.rotMatrix.rot(this.rotX, this.rotY, this.rotZ)
			this.cnt+= 0.02;
		}
                this.particles.forEach(function(particle){
			if (IsMouseButtonPressed(MOUSE_LEFT)) {
				this.rotMatrix.xParticle(particle);
			}
			else particle.move();

			var zoom = 600/(50+particle.v.z);
			var px = (particle.v.x*zoom);
			var py = (particle.v.y*zoom);
			particle.v.px = px;
			particle.v.py = py;

			var lx = particle.v.px - particle.oldV.px;
			var ly = particle.v.py - particle.oldV.py;
			particle.length = Math.sqrt( lx*lx + ly*ly );

			var size = 1;

                        particle.Group.setX(px);
                        particle.Group.setY(py);
                        particle.Group.draw();
		}, this);

	}

	this.onMouseMove = function(){
		this.gV.x = ((GetMouseX())-SCREEN_OX)/12;
		this.gV.y = ((GetMouseY())-SCREEN_OY)/12;
	}

	this.particleCompare = function(p1, p2){
		return (p2.v.z - p1.v.z);
	}

}

function game(){

    var Demo = new ParticleDemo();
    Demo.initParticles();
    while(!IsKeyPressed(KEY_Q)){

        var t = GetTime();

        Demo.loop();
        Demo.onMouseMove();
        FlipScreen();
        Delay(Math.max(0, GetTime()-t+12));

    }

}

