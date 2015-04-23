

#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>
#include <gl/freeglut.h>
#include "LoadShaders.h"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;
using namespace std;

mat4	matMV;
mat4	matP;

enum eVertexArrayObject
{
	VAOCtrlPts,	
	VAOCount
};

enum eVertexBufferObject
{
	VBOCtrlPts,	
	VBOCount
};

enum eShader
{
	HermiteCurveShader,
	FixedFuncShader,
	ShaderCount
};

GLuint vao[VAOCount];
GLuint vbo[VBOCount];
GLuint glsl[ShaderCount];

GLfloat control_pt[][3] =
{
	{ -2.f, -3.f, 0.f },	
	{ 3.f, 2.f, 0.f }
};

GLfloat tangent_vector[][3] =
{
	{ -1.f,   5.0f, 0.f},
	{ -1.f, 3.f, 0.f}
};

const double WORLD_SIZE = 4.;

void initHermiteCurveShader()
{
	ShaderInfo shader_info[] =
    {
        { GL_VERTEX_SHADER, "../src/glsl/hermite_curve.vert" },
		{ GL_TESS_CONTROL_SHADER, "../src/glsl/hermite_curve.tess" },
		{ GL_TESS_EVALUATION_SHADER, "../src/glsl/hermite_curve.eval" },
        { GL_FRAGMENT_SHADER, "../src/glsl/hermite_curve.frag" },
        { GL_NONE, NULL }
    };

	glsl[HermiteCurveShader] = LoadShaders(shader_info);

	//////////////////////////////
	// set control points
	glBindVertexArray( vao[ VAOCtrlPts] );
	glBindBuffer( GL_ARRAY_BUFFER, vbo[ VBOCtrlPts ] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(control_pt), control_pt, GL_STATIC_DRAW );

	glBindBuffer( GL_ARRAY_BUFFER, 0 );	
	glBindVertexArray( 0 );
}

void initFixedFuncShader()
{
	ShaderInfo ff_shader_info[] =
	{ 
		{ GL_VERTEX_SHADER, "../src/glsl/vertex.glsl" },
		{ GL_FRAGMENT_SHADER, "../src/glsl/fragment.glsl" },
		{ GL_NONE, NULL }
	};

	glsl[FixedFuncShader] = LoadShaders( ff_shader_info );	
}

void initGL()
{	
	glClearColor(0.7, 0.7, 0.7, 1.);

	glPointSize(3.);
	glEnable (GL_LINE_SMOOTH);
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint (GL_LINE_SMOOTH_HINT, GL_DONT_CARE);

	glGenVertexArrays( VAOCount, &vao[ 0 ] );
	glGenBuffers( VBOCount, &vbo[ 0 ] );

	initHermiteCurveShader();
	initFixedFuncShader();

	glBindBuffer( GL_ARRAY_BUFFER, 0 );	
	glBindVertexArray( 0 );
	
}

void cbReshape(int w, int h)
{
	float ar = (float) w / h;
	glViewport(0,0, w,h);
	matP = glm::ortho( -WORLD_SIZE*ar, WORLD_SIZE*ar, -WORLD_SIZE, WORLD_SIZE, -1., 1. );
	matMV = glm::mat4(1.);
}

void cbKey(unsigned char c, int w, int h )
{
	switch(c)
	{	
		case 27:case'q':
			glutExit();
			break;
	}
}

void drawHermiteCurve()
{
	glUseProgram( glsl[HermiteCurveShader] );

	GLuint locMatP = glGetUniformLocation( glsl[HermiteCurveShader], "matProjection" );
	GLuint locMatMV = glGetUniformLocation( glsl[HermiteCurveShader], "matModelView" );
	glUniformMatrix4fv( locMatP, 1, GL_FALSE, glm::value_ptr( matP ) );
	glUniformMatrix4fv( locMatMV, 1, GL_FALSE, glm::value_ptr( matMV ) );

	GLuint locTan0 = glGetUniformLocation( glsl[HermiteCurveShader], "vTan0" );
	GLuint locTan1 = glGetUniformLocation( glsl[HermiteCurveShader], "vTan1" );
	glUniform3fv( locTan0, 1, &tangent_vector[0][0] );
	glUniform3fv( locTan1, 1, &tangent_vector[1][0] );

	glBindVertexArray( vao[ VAOCtrlPts ] );
	glBindBuffer( GL_ARRAY_BUFFER, vbo[ VBOCtrlPts ] );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 );
	glEnableVertexAttribArray( 0 );
	
	glLineWidth( 1.5f );
	glPatchParameteri( GL_PATCH_VERTICES, 2 );
	glDrawArrays( GL_PATCHES, 0, 2 );
	
	glUseProgram( glsl[FixedFuncShader] );
	locMatP = glGetUniformLocation( glsl[FixedFuncShader], "matProjection" );
	locMatMV = glGetUniformLocation( glsl[FixedFuncShader], "matModelView" );
	glUniformMatrix4fv( locMatP, 1, GL_FALSE, glm::value_ptr( matP ) );
	glUniformMatrix4fv( locMatMV, 1, GL_FALSE, glm::value_ptr( matMV ) );

	glPushAttrib( GL_LINE_BIT );
	glLineWidth( 0.1f );
	glDrawArrays( GL_LINES, 0, 2 );
	glPopAttrib();
	glDrawArrays( GL_POINTS, 0, 2 );
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glDisableVertexAttribArray( 0 );
}

void cbDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	drawHermiteCurve();
	
	glutSwapBuffers(); 
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE );
	glutInitWindowSize(512, 512 );
	glutInitContextVersion(4, 4);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("Hermite Curve");
	glutReshapeFunc(cbReshape);
	glutKeyboardFunc(cbKey);
	glutDisplayFunc(cbDisplay);
	glewExperimental = GL_TRUE; 
	glewInit();
	initGL();
	
	glutMainLoop();
	return 0;
}
