﻿#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>
#include "glext.h"
#include "colors.h"
#include "targa.h"
#include <cassert>

PFNGLWINDOWPOS2IPROC glWindowPos2i = nullptr;

enum
{
	FULL_WINDOW,
	ASPECT_1_1,
	EXIT
};

int aspect = FULL_WINDOW;

#ifdef near
#undef near
#endif
#ifdef far
#undef far
#endif

const GLdouble left = -2.0;
const GLdouble right = 2.0;
const GLdouble bottom = -2.0;
const GLdouble top = 2.0;
const GLdouble near = 3.0;
const GLdouble far = 7.0;

GLfloat rotatex = 270.0;
GLfloat rotatez = 0.0;
int button_state = GLUT_UP;
int button_x, button_y;
GLfloat scale = 1.5;
GLuint texture;
GLint min_filter = GL_LINEAR_MIPMAP_LINEAR;

void DrawString(GLint x, GLint y, char *string)
{
	if (glWindowPos2i == nullptr)
		return;

	glWindowPos2i(x, y);

	int len = strlen(string);
	for (int i = 0; i < len; i++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, string[i]);
}

void DisplayScene()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -(near + far) / 2);
	glRotatef(rotatex, 1.0, 0.0, 0.0);
	glRotatef(rotatez, 0.0, 0.0, 1.0);
	glScalef(scale, scale, scale);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	GLUquadricObj *quadobj = gluNewQuadric();
	gluQuadricDrawStyle(quadobj, GLU_FILL);
	gluQuadricNormals(quadobj, GLU_SMOOTH);
	gluQuadricTexture(quadobj, GL_TRUE);
	gluSphere(quadobj, 1.0, 30, 30);
	gluDeleteQuadric(quadobj);
	glDisable(GL_TEXTURE_2D);


	char string[200];
	GLfloat var;
	glColor3fv(White);


	glGetTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_PRIORITY, &var);
	sprintf(string, "GL_TEXTURE_PRIORITY = %f", var);
	DrawString(2, 2, string);


	glGetTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_RESIDENT, &var);
	if (var == GL_FALSE)
		strcpy(string, "GL_TEXTURE_RESIDENT = GL_FALSE");
	else
		strcpy(string, "GL_TEXTURE_RESIDENT = GL_TRUE");
	DrawString(2, 16, string);


	glGetTexLevelParameterfv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &var);
	sprintf(string, "GL_TEXTURE_WIDTH = %f", var);
	DrawString(2, 30, string);


	glGetTexLevelParameterfv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &var);
	sprintf(string, "GL_TEXTURE_HEIGHT = %f", var);
	DrawString(2, 46, string);

	glFlush();
	glutSwapBuffers();
}

void Reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (aspect == ASPECT_1_1)
	{
		if (width < height && width > 0)
			glFrustum(left, right, bottom*height / width, top*height / width, near, far);
		else if (width >= height && height > 0)
			glFrustum(left*width / height, right*width / height, bottom, top, near, far);
	}
	else
		glFrustum(left, right, bottom, top, near, far);

	DisplayScene();
}

void Keyboard(unsigned char key, int x, int y)
{

	if (key == '+')
		scale += 0.05;
	else if (key == '-' && scale > 0.05)
		scale -= 0.05;

	DisplayScene();
}

void MouseButton(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{

		button_state = state;
		if (state == GLUT_DOWN)
		{
			button_x = x;
			button_y = y;
		}
	}
}

void MouseMotion(int x, int y)
{
	if (button_state == GLUT_DOWN)
	{
		rotatez += 30 * (right - left) / glutGet(GLUT_WINDOW_WIDTH) * (x - button_x);
		button_x = x;
		rotatex -= 30 * (top - bottom) / glutGet(GLUT_WINDOW_HEIGHT) * (button_y - y);
		button_y = y;
		glutPostRedisplay();
	}
}

void Menu(int value)
{

	switch (value)
	{
	case GL_NEAREST_MIPMAP_NEAREST:
	case GL_NEAREST_MIPMAP_LINEAR:
	case GL_LINEAR_MIPMAP_NEAREST:
	case GL_LINEAR_MIPMAP_LINEAR:
		min_filter = value;
		DisplayScene();
		break;


	case FULL_WINDOW:
		aspect = FULL_WINDOW;
		Reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
		break;


	case ASPECT_1_1:
		aspect = ASPECT_1_1;
		Reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
		break;


	case EXIT:
		exit(0);
	}
}

void GenerateTextures()
{

	GLsizei width, height;
	GLenum format, type;
	GLvoid* pixels;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// ---------------------- nazwa pliku
	GLboolean error = load_targa("saturn.tga", width, height, format, type, pixels);
	if (error == GL_FALSE)
	{
		printf("Niepoprawny odczyt pliku ven0aaa2.tga");
		exit(0);
	}

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, width, height, format, type, pixels);

	delete[] pixels;
}

void ExtensionSetup()
{
	const char *version = (char*)glGetString(GL_VERSION);
	int major = 0, minor = 0;
	if (sscanf(version, "%d.%d", &major, &minor) != 2)
	{
		printf("Błędny format wersji OpenGL\n");

		exit(0);
	}

	if (major > 1 || minor >= 4)
	{
		glWindowPos2i = (PFNGLWINDOWPOS2IPROC)wglGetProcAddress("glWindowPos2i");
	}
	else if (glutExtensionSupported("GL_ARB_window_pos"))
	{
		glWindowPos2i = (PFNGLWINDOWPOS2IPROC)wglGetProcAddress("glWindowPos2iARB");
	}
	else
	{
		printf("Brak rozszerzenia ARB_window_pos!\n");
		exit(0);
	}
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutCreateWindow("Saturn");
	glutDisplayFunc(DisplayScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(MouseButton);
	glutMotionFunc(MouseMotion);

	int MenuMinFilter = glutCreateMenu(Menu);
	glutAddMenuEntry("GL_NEAREST_MIPMAP_NEAREST", GL_NEAREST_MIPMAP_NEAREST);
	glutAddMenuEntry("GL_NEAREST_MIPMAP_LINEAR", GL_NEAREST_MIPMAP_LINEAR);
	glutAddMenuEntry("GL_LINEAR_MIPMAP_NEAREST", GL_LINEAR_MIPMAP_NEAREST);
	glutAddMenuEntry("GL_LINEAR_MIPMAP_LINEAR", GL_LINEAR_MIPMAP_LINEAR);


	int MenuAspect = glutCreateMenu(Menu);
	glutAddMenuEntry("Aspekt obrazu - cale okno", FULL_WINDOW);
	glutAddMenuEntry("Aspekt obrazu 1:1", ASPECT_1_1);
	glutCreateMenu(Menu);
	glutAddSubMenu("Filtr pomniejszajacy", MenuMinFilter);
	glutAddSubMenu("Aspekt obrazu", MenuAspect);
	glutAddMenuEntry("Wyjscie", EXIT);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	GenerateTextures();
	ExtensionSetup();
	glutMainLoop();
	return 0;
}