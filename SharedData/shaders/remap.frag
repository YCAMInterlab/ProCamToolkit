uniform sampler2DRect remap, scene, shoji, lut;
uniform float weight;
uniform vec2 sceneOffset;
uniform float sceneScale;

const float topBoundary = 2.; // ignore the top couple pixels
const vec2 remapSize = vec2(1., .75);
const vec2 sceneSize = vec2(1024., 1024.);
const float lutSize = 256.;

void main(void) {
	vec2 target = (texture2DRect(remap, gl_TexCoord[0].st).rg * remapSize - sceneOffset) / sceneScale;
	if(target.x > 1. || target.x < 0. ||
			target.y > 1. || target.y < 0. ||
			gl_TexCoord[0].t < topBoundary) {
		gl_FragColor = vec4(0, 0, 0, 1);
	} else {
		target *= sceneSize;
		vec4 mul;
		if(weight > 0.) {
			mul = texture2DRect(shoji, target);
		} else {
			mul = 1. - texture2DRect(shoji, target);
		}
		gl_FragColor = texture2DRect(scene, target) * mul;
		gl_FragColor = vec4(
			texture2DRect(lut, vec2(lutSize * gl_FragColor.r, 0.)).r,
			texture2DRect(lut, vec2(lutSize * gl_FragColor.g, 0.)).g,
			texture2DRect(lut, vec2(lutSize * gl_FragColor.b, 0.)).b,
			1.);
	}
}