#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
uniform mat4 model;
uniform mat4 camMatrix;
out vec2 texCoord;
void main(){
	texCoord = aTexCoord;
	gl_Position = camMatrix*model*vec4(aPos,1);
}