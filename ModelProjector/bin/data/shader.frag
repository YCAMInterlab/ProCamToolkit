#define PI (3.1415926536)
#define TWO_PI (6.2831853072)

uniform int stage;

uniform float elapsedTime;
varying vec3 position, normal;
varying float randomOffset;

const vec4 on = vec4(1.);
const vec4 off = vec4(vec3(0.), 1.);

uniform sampler2DRect tex0, tex1, tex2, tex3;

void main() {
	if(stage == 0) {
		// diagonal stripes
		const float speed = 50.;
		const float scale = 50.;
		gl_FragColor = 
			(mod((position.x + position.y + position.z) + (elapsedTime * speed), scale) > scale / 2.) ?
			on : off;
	} else if(stage == 1) {
		// crazy color bounce
		gl_FragColor = vec4(position / 100. * sin(mod(elapsedTime, TWO_PI)), 1.);
	} else if(stage == 2) {
		// fast rising stripes
		if(normal.z == 0.) {
			const float speed = 60.;
			const float scale = 30.;
			gl_FragColor = 
				(mod((-position.z) + (elapsedTime * speed), scale) < 1.) ?
				on : off;
		} else {
			gl_FragColor = off;
		}
	} else if(stage == 3 || stage == 4) {
		// crazy triangles, grid lines
		float speed = 10.;
		float scale = 50.0;	
		float cutoff = .9;
		vec3 cur = mod(position + speed * elapsedTime, scale) / scale;
		cur *= 1. - abs(normal);
		if(stage == 3) {
			gl_FragColor = ((cur.x + cur.y + cur.z) < cutoff) ? off : on;
		} else {
			gl_FragColor = (max(max(cur.x, cur.y), cur.z) < cutoff) ? off : on;
		}
	} else if(stage == 5) {
		// moving outlines
		const float speed = 100.;
		const float scale = 6000.;
		float localTime = 5. * randomOffset + elapsedTime;
		gl_FragColor = 
			(mod((-position.x - position.y + position.z) + (localTime * speed), scale) > scale / 2.) ?
			on : off;
	} else if(stage == 6) {
		// spinning (outline or face) 
		vec2 divider = vec2(cos(elapsedTime), sin(elapsedTime));
		float side = (position.x * divider.y) - (position.y * divider.x);
		gl_FragColor = abs(side) < 300. + 300. * sin(elapsedTime * .3) ? on : off;
	} else {
		
	}
}
