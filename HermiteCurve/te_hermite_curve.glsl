#version 440 core

layout (isolines, equal_spacing, ccw) in;

uniform mat4  matModelView;
uniform mat4  matProjection;

uniform vec3 vTan0;
uniform vec3 vTan1;

///////////////////////////////////////////////////
// function to calculate the hermite function value
vec3 hermite(float u, vec3 p0, vec3 p1, vec3 t0, vec3 t1)
{
	float F1 = 2.*u*u*u - 3.*u*u + 1.;
	float F2 = -2.*u*u*u + 3*u*u;
	float F3 = u*u*u - 2.*u*u + u;
	float F4 = u*u*u - u*u;

	vec3 p = F1*p0 + F2*p1 + F3*t0 + F4*t1;
	return p;
} 

void
main()
{
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

	vec3 vPos0 = vec3( gl_in[0].gl_Position );
	vec3 vPos1 = vec3( gl_in[1].gl_Position );
	vec3 v3pos = hermite( u, vPos0, vPos1, vTan0, vTan1 );
	vec4 pos = vec4( v3pos, 1.);
	gl_Position = matProjection * matModelView * pos;
}
