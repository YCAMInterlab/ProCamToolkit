uniform sampler2DRect points;
uniform int count;

const float banding = 10.;
const float lineWidth = .33;

vec2 getPoint(int i) {
	// need .5 offset to lookup the middle of the bin
	vec2 st = vec2(.5 + float(i), 0.);
	return texture2DRect(points, st).xy;
}

float xorMix(bool a, bool b) {
	return a ? (b ? .5 : 1.) : (b ? 0. : .5);
}

void main(void) {
	vec2 p = gl_TexCoord[0].xy;
	
	vec2 start, end;
	float t;
	
	end = getPoint(0);
  for(int i = 1; i < count; i++) {
		start = end;
		end = getPoint(i);
		vec2 b = end - start;
		vec2 c = p - start;
    if((b.x * c.y) - (b.y * c.x) > 0.) {
      continue;
		}
    t = dot(b, c) / dot(b, b);
    p = (b * t + start) * 2. - p;
  }
	
	p = mod(p * banding, 1.);
	float b = xorMix(p.x < lineWidth, p.y < lineWidth);
	gl_FragColor = vec4(b, b, b, 1.);
}