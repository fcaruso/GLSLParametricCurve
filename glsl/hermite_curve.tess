#version 440 core

layout (vertices = 4) out;

void
main()
{
    gl_TessLevelOuter[0] = 1;
    gl_TessLevelOuter[1] = 16;
	
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}
