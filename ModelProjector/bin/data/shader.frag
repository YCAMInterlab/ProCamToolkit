uniform float elapsedTime;
varying vec3 position, normal;

void main() {
	if(mod(position.z + elapsedTime * 10., 20.) > 10. ||
		mod(position.y + elapsedTime * 10., 20.) > 10.) {
		gl_FragColor = vec4(1.);
	} else {
		gl_FragColor = vec4(0., 0., 0., 1.);
	}
}
