varying vec3 position, normal;

void main() {
	gl_FrontColor = gl_Color;
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	position = gl_Vertex.xyz;
	normal = gl_Normal.xyz;
}
