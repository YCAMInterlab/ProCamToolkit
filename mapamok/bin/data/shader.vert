varying vec3 position, normal;
varying float randomOffset;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main() {
	gl_FrontColor = gl_Color;
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	position = gl_Vertex.xyz;
	normal = gl_Normal.xyz;
	randomOffset = rand(gl_Vertex.xy + gl_Vertex.yz);
}
