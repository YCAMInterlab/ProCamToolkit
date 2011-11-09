uniform float elapsedTime;
varying vec3 position, normal;

const vec4 on = vec4(1.);
const vec4 off = vec4(0., 0., 0., 1.);
const float speed = 50.;
const float scale = 50.;

void main() {
	if(mod((position.x + position.y + position.z) + (elapsedTime * speed), scale) > (scale / 2.)) {
		gl_FragColor = on;
	} else {
		gl_FragColor = off;
	}
}
