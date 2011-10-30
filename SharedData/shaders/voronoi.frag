uniform sampler2DRect points;
uniform int count;
const float TWO_PI = 6.283185;

vec4 getPoint(int i) {
	// need .5 offset to lookup the middle of the bin
	vec2 st = vec2(.5 + float(i), 0.);
	return texture2DRect(points, st);
}

void main(void) {
	vec2 st = gl_TexCoord[0].st;
	int minIndex = 0;
	float minDistance = 0.;
	for(int i = 0; i < count; i++) {
		vec4 cur = getPoint(i);
		float dx = cur.x - st.x;
		float dy = cur.y - st.y;
		
		// euclidian distance
		float distance = dx * dx + dy * dy;
		// manhattan distance
		//float distance = abs(dx) + abs(dy);
		// psychedelic distance
		//float distance = sin((sin(TWO_PI * dx) + sin(TWO_PI * dy)) * TWO_PI);
		
		if(i == 0 || distance < minDistance) {
			minDistance = distance;
			minIndex = i;
		}
	}
	float b = getPoint(minIndex).b;
	
	// standard shading
	gl_FragColor = vec4(b, b, b, 1);
	// circular patterns
	//gl_FragColor = vec4(mod(st.x + st.y + mod(minDistance * 100., 1.) + b, 1.));
	// crazy colors
	//gl_FragColor = vec4(st.x, mod(minDistance * 10., 1.), b, 1);
}