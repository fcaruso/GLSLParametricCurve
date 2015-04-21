#version 440 core

uniform mat4 matModelView;
uniform mat4 matProjection;

layout (location=0) in vec4 vPosition;

void
main()
{
	gl_Position = matProjection*matModelView*vPosition;
}