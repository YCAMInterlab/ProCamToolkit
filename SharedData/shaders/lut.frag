uniform sampler2DRect lut, source;

const float lutSize = 256.;

void main(void) {
	vec2 st = gl_TexCoord[0].st;
	vec4 c = texture2DRect(source, st);
	gl_FragColor = vec4(
		texture2DRect(lut, vec2(lutSize * c.r, 0.)).r,
		texture2DRect(lut, vec2(lutSize * c.g, 0.)).g,
		texture2DRect(lut, vec2(lutSize * c.b, 0.)).b,
		c.a);
}