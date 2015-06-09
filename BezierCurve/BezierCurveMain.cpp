/*

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>
*/


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
	VAOCurve,
	VAOCount
};

enum eVertexBufferObject
{
	VBOCurve,
	VBOCount
};

enum eShader
{
	BezierCurveShader,
	FixedFuncShader,
	ShaderCount
};

GLuint vao[VAOCount];
GLuint vbo[VBOCount];
GLuint glsl[ShaderCount];

GLfloat curve_pts[][3] =
{
	{ -4.f, -3.f, 0.f },
	{ -2.f, 3.f, 0.f },
	{ 3.f, 2.f, 0.f },
	{ 4.f,-1.f, 0.f }
};

const double WORLD_SIZE = 5.;

void initBezierCurveShader()
{
	ShaderInfo shader_info[] =
    {
        { GL_VERTEX_SHADER, "../src/glsl/bezier_curve.vert" },
		{ GL_TESS_CONTROL_SHADER, "../src/glsl/bezier_curve.tess" },
		{ GL_TESS_EVALUATION_SHADER, "../src/glsl/bezier_curve.eval" },
        { GL_FRAGMENT_SHADER, "../src/glsl/bezier_curve.frag" },
        { GL_NONE, NULL }
    };

	glsl[BezierCurveShader] = LoadShaders(shader_info);

	glBindVertexArray( vao[ VAOCurve ] );
	glBindBuffer( GL_ARRAY_BUFFER, vbo[ VBOCurve ] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(curve_pts), curve_pts, GL_STATIC_DRAW );
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

	initBezierCurveShader();
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

void drawBezierCurve()
{
	glUseProgram( glsl[BezierCurveShader] );

	GLuint locMatP = glGetUniformLocation( glsl[BezierCurveShader], "matProjection" );
	GLuint locMatMV = glGetUniformLocation( glsl[BezierCurveShader], "matModelView" );
	glUniformMatrix4fv( locMatP, 1, GL_FALSE, glm::value_ptr( matP ) );
	glUniformMatrix4fv( locMatMV, 1, GL_FALSE, glm::value_ptr( matMV ) );

	glBindVertexArray( vao[ VAOCurve ] );
	glBindBuffer( GL_ARRAY_BUFFER, vbo[ VBOCurve ] );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 );
	glEnableVertexAttribArray( 0 );

	glPatchParameteri( GL_PATCH_VERTICES, 4 );
	glDrawArrays( GL_PATCHES, 0, 4 );
	
	glUseProgram( glsl[FixedFuncShader] );
	locMatP = glGetUniformLocation( glsl[FixedFuncShader], "matProjection" );
	locMatMV = glGetUniformLocation( glsl[FixedFuncShader], "matModelView" );
	glUniformMatrix4fv( locMatP, 1, GL_FALSE, glm::value_ptr( matP ) );
	glUniformMatrix4fv( locMatMV, 1, GL_FALSE, glm::value_ptr( matMV ) );

	glDrawArrays( GL_LINE_STRIP, 0, 4 );
	glDrawArrays( GL_POINTS, 0, 4 );
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glDisableVertexAttribArray( 0 );
}

void cbDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	drawBezierCurve();
	
	glutSwapBuffers(); 
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE );
	glutInitWindowSize(512, 512 );
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("Bezier Curve");
	glutReshapeFunc(cbReshape);
	glutKeyboardFunc(cbKey);
	glutDisplayFunc(cbDisplay);
	glewExperimental = GL_TRUE; 
	glewInit();
	initGL();
	
	glutMainLoop();
	return 0;
}
