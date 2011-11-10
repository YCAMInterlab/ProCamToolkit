uniform float elapsedTime;
varying vec3 position, normal;

const vec4 on = vec4(1.);
const vec4 off = vec4(vec3(0.), 1.);
const float speed = 50.;
const float scale = 50.;

void main() {
	float stages = 2.;
	float stage = 2.;// mod(elapsedTime, stages);
	if(stage < 1.) {
		if(mod((position.x + position.y + position.z) + (elapsedTime * speed), scale) > (scale / 2.)) {
			gl_FragColor = on;
		} else {
			gl_FragColor = off;
		}
	} else if(stage < 2.) {
		gl_FragColor = on;
	}
}
