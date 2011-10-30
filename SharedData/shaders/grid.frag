uniform sampler2DRect points;
uniform int count;

const int subdivisions = 16;
const float lineWidth = .5;

const float TWO_PI = 6.28318531;
// res has to be an integer divided by TWO_PI
// it can be scaled by count, but has to be scaled using an integer divide
const float res = float(subdivisions) / TWO_PI;

vec4 getPoint(int i) {
	// need .5 offset to lookup the middle of the bin
	vec2 st = vec2(.5 + float(i), 0.);
	return texture2DRect(points, st);
}

float xorMix(bool a, bool b) {
	return a ? (b ? .5 : 1.) : (b ? 0. : .5);
}

void main(void) {
	vec2 xy = gl_TexCoord[0].xy;
	vec2 field = vec2(0, 0);
	for(int i = 0; i < count; i++) {
		vec2 d = xy - getPoint(i).xy;
		field.x += atan(d.y, d.x);
		field.y += log(length(d));
	}
	vec2 mfield = mod(field * res, 1.);
	float b = xorMix(mfield.x < lineWidth, mfield.y < lineWidth);
	gl_FragColor = vec4(b, b, b, 1.);
}