#version 440 core

uniform mat4 matModelView;
uniform mat4 matProjection;

uniform vec3 vTan0;
uniform vec3 vTan1;

layout (location=0) in vec4 vPosition;

void
main()
{
	gl_Position = vPosition;
}