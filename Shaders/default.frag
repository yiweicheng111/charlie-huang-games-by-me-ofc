#version 330 core
out vec4 FragColor;
uniform sampler2D colorMap;
uniform vec3 color;
in vec2 texCoord;
uniform bool usesColorMap;
void main(){
	 if (usesColorMap){
	 		FragColor = texture(colorMap,texCoord);
	 }
	 else{
		FragColor = vec4(1,0,0,1);
	 }
}