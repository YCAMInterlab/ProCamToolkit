uniform sampler2DRect points;
uniform int count;

const float TWO_PI = 6.28318530;

const float spacing = .1;
const float lineWidth = .5;
const float strength = .01;
const float cutoff = spacing * lineWidth;
const float offset = (spacing + cutoff) / 2.;

vec4 getPoint(int i) {
	// need .5 offset to lookup the middle of the bin
	vec2 st = vec2(.5 + float(i), 0.);
	return texture2DRect(points, st);
}

float xorMix(bool a, bool b) {
	return a ? (b ? .5 : 1.) : (b ? 0. : .5);
}

void main(void) {
	vec2 st = gl_TexCoord[0].st;
	vec2 sto = st;
	
	for(int i = 0; i < count; i++) {
		vec4 cur = getPoint(i);
		vec2 d = cur.st - st;
		
		// euclidian distance
		float distance = sqrt(d.x * d.x + d.y * d.y);
		// manhattan distance
		//float distance = abs(d.x) + abs(d.y);
		// psychedelic distance
		//float distance = sin((sin(TWO_PI * d.x) + sin(TWO_PI * d.y)) * TWO_PI);
		
		float displacement = strength / (distance * distance);
		if(displacement < distance) {
			sto += normalize(d) * displacement;
		} else {
			gl_FragColor = vec4(0., 0., 0., 1.);
			return;
		}
	}

	vec2 m = mod(sto + offset, spacing);
	float b = xorMix(m.x < cutoff, m.y < cutoff);
	gl_FragColor = vec4(b, b, b, 1.);
}