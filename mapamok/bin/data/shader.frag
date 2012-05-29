#define PI (3.1415926536)
#define TWO_PI (6.2831853072)

uniform float elapsedTime;
varying vec3 position, normal;
varying float randomOffset;

const vec4 on = vec4(1.);
const vec4 off = vec4(vec3(0.), 1.);

void main() {
	float stages = 6.;
	float stage = mod(elapsedTime * .6, stages);
	if(stage < 1.) {
		// diagonal stripes
		const float speed = 50.;
		const float scale = 50.;
		gl_FragColor = 
			(mod((position.x + position.y + position.z) + (elapsedTime * speed), scale) > scale / 2.) ?
			on : off;
	} else if(stage < 2.) {
		// crazy color bounce
		gl_FragColor = vec4(mod(elapsedTime + position / 100., 1.) * sin(mod(elapsedTime * 4., TWO_PI)), 1.);
	} else if(stage < 3.) {
		// fast rising stripes
		//if(normal.z == 0.) {
			const float speed = 200.;
			const float scale = 50.;
			gl_FragColor = 
				(mod((-position.z) + (elapsedTime * speed), scale) < (scale / 2.)) ?
				on : off;
		/*} else {
			gl_FragColor = off;
		}*/
	} else if(stage < 5.) {
		// crazy triangles, grid lines
		float speed = 10.;
		float scale = 50.0;	
		float cutoff = .9;
		vec3 cur = mod(position + speed * elapsedTime, scale) / scale;
		cur *= 1. - abs(normal);
		if(stage < 4.) {
			gl_FragColor = ((cur.x + cur.y + cur.z) < cutoff) ? off : on;
		} else {
			gl_FragColor = (max(max(cur.x, cur.y), cur.z) < cutoff) ? off : on;
		}
	} else if(stage < 6.) {
		// moving outlines
		const float speed = 100.;
		const float scale = 6000.;
		float localTime = 5. * randomOffset + elapsedTime;
		gl_FragColor = 
			(mod((-position.x - position.y + position.z) + (localTime * speed), scale) > scale / 2.) ?
			on : off;
	} else if(stage < 7.) {
		// spinning (outline or face) 
		vec2 divider = vec2(cos(elapsedTime), sin(elapsedTime));
		float side = (position.x * divider.y) - (position.y * divider.x);
		gl_FragColor = abs(side) < 100. + 280. * sin(elapsedTime * 1.) ? on : off;
	}
}
