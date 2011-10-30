uniform sampler2DRect tex;
uniform float radius;

const float total = (1. + 8. + 28. + 56.) * 2. + 70.;

void main(void) {
	vec2 st = gl_TexCoord[0].st;

	gl_FragColor = (1. / total) * texture2DRect(tex, st - radius * vec2(0., 4. / 4.));
	gl_FragColor += (8. / total)  * texture2DRect(tex, st - radius * vec2(0., 3. / 4.));
	gl_FragColor += (28. / total)  * texture2DRect(tex, st - radius * vec2(0., 2. / 4.));
	gl_FragColor += (56. / total)  * texture2DRect(tex, st - radius * vec2(0., 1. / 4.));

	gl_FragColor +=  (70. / total) * texture2DRect(tex, st);
	
	gl_FragColor += (1. / total) * texture2DRect(tex, st + radius * vec2(0., 4. / 4.));
	gl_FragColor += (8. / total)  * texture2DRect(tex, st + radius * vec2(0., 3. / 4.));
	gl_FragColor += (28. / total)  * texture2DRect(tex, st + radius * vec2(0., 2. / 4.));
	gl_FragColor += (56. / total)  * texture2DRect(tex, st + radius * vec2(0., 1. / 4.));
}