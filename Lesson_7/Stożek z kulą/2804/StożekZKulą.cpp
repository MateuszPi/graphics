#include <GL/glut.h>
#include <stdlib.h>
#include "colors.h"
#include "gltools_extracted.h"
#include "VectorMath.cpp"
#include <array>
#include <vector>

const GLint N = 7;

// rozmiary bry�y obcinania
const GLdouble left = -2.0;
const GLdouble right = 2.0;
const GLdouble bottom = -2.0;
const GLdouble top = 2.0;
const GLdouble near_ = 3.0;
const GLdouble far_ = 7.0;

// sta�e do obs�ugi menu podr�cznego
enum
{
	CUTTING_PLANE, // p�aszczyzna przekroju
	POLYGON_OFFSET, // przesuni�cie warto�ci g��bi
	EXIT // wyj�cie
};

GLfloat angle = 0.0; // k�t obrotu kuli

// k�ty obrotu sze�cianu
GLfloat rotatex = 0.0;
GLfloat rotatey = 0.0;

// wska�nik rysowania p�aszczyzna przekroju
bool cutting_plane = true;

bool polygon_offset = true; // wska�nik przesuni�cia warto�ci g��bi
int button_state = GLUT_UP; // wska�nik naci�ni�cia lewego przycisku myszki
int button_x, button_y; // po�o�enie kursora myszki

void drawPyramid(GLuint n, GLfloat size, GLenum type, bool drawBase)
{
	GLTVector3 topVertice{ 0.0f, +size, 0.0f };
	GLTVector3 baseCenter{ 0.0f, -size/2.0, 0.0f };

	std::vector<GLTVector3> bottomVertices{ n };

	for (int i = 0; i < n; ++i)
	{
		const GLfloat angle = (GLfloat)i / n * 3.1415926 * 2.0;

		bottomVertices[i][0] = size * sin(angle);
		bottomVertices[i][1] = -size / 2;
		bottomVertices[i][2] = size * cos(angle);
	}

	if (drawBase)
	{
		for (int i = 0; i <= n; ++i)
		{
			glBegin(type);
			glNormal3f(0.0f, -1.0f, 0.0f);
			glVertex3fv(baseCenter);
			glVertex3fv(bottomVertices[(i + 1) % n]);
			glVertex3fv(bottomVertices[i % n]);
			glEnd();
		}
	}
	for (int i = 0; i <= n; ++i)
	{
		glBegin(type);
		GLTVector3 normal;
		gltGetNormalVector(bottomVertices[i % n], bottomVertices[(i + 1) % n], topVertice, normal);
		glNormal3fv(normal);

		glVertex3fv(bottomVertices[i % n]);
		glVertex3fv(bottomVertices[(i + 1) % n]);
		glVertex3fv(topVertice);

		glEnd();
	}
}

void drawSolidPyramid(GLint n, GLdouble size)
{
	drawPyramid(n, size, GL_TRIANGLES, true);
}

void drawWirePyramid(GLint n, GLdouble size)
{
	drawPyramid(n, size, GL_LINE_LOOP, false);
}

// funkcja generuj�ca scen� 3D
void Display()
{
	glClearColor(1.0, 1.0, 1.0, 1.0); // kolor t�a - zawarto�� bufora koloru
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // czyszczenie bufora koloru i bufora g��boko�ci
	glMatrixMode(GL_MODELVIEW); // wyb�r macierzy modelowania
	glLoadIdentity(); // macierz modelowania = macierz jednostkowa
	glTranslatef(0, 0, -(near_ + far_) / 2); // przesuni�cie uk�adu wsp�rz�dnych sze�cianu do �rodka bry�y odcinania

	// obroty sze�cianu
	glRotatef(rotatex, 1.0, 0, 0);
	glRotatef(rotatey, 0, 1.0, 0);

	//glScalef(1.15, 1.15, 1.15); // niewielkie powi�kszenie sze�cianu
	glEnable(GL_DEPTH_TEST); // w��czenie testu bufora g��boko�ci

	// najpierw rysujemy kul� obracaj�c� si� wewn�trz sze�cianu;
	// z uwagi na celowy brak efekt�w o�wietlenia, obr�t kuli
	// podkre�la druga kula w wersji "szkieletowej"
	glPushMatrix();
	angle += 0.2;
	glRotatef(angle, 1.0, 1.0, 0.0);
	glColor3fv(Yellow);
	if (polygon_offset)
		glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0, 1.0);
	glutSolidSphere(0.5, 10, 10);
	glColor3fv(Black);
	glutWireSphere(0.5, 10, 10);
	if (polygon_offset)
		glDisable(GL_POLYGON_OFFSET_FILL);
	glPopMatrix();


	const GLfloat a = 0.5;

	// w drugiej kolejno�ci rysujemy wn�trze sze�cianu;
	// rysowane s� tylko przednie strony wewn�trznych �cian
	// no i nie jest rysowana �ciana przednia, w kt�rej b�dzie otw�r
	glEnable(GL_CULL_FACE);
	glBegin(GL_QUADS);
	glColor3fv(Blue);
	glVertex3f(-a, -2*a, -a);
	glVertex3f(a, -2 * a, -a);
	glVertex3f(a, a, -a);
	glVertex3f(-a, a, -a);
	glColor3fv(Magenta);
	glVertex3f(-a, a, a);
	glVertex3f(a, a, a);
	glVertex3f(a, -2 * a, a);
	glVertex3f(-a, -2 * a, a);
	glColor3fv(Lime);
	glVertex3f(-a, a, a);
	glVertex3f(-a, -2 * a, a);
	glVertex3f(-a, -2 * a, -a);
	glVertex3f(-a, a, -a);
	glColor3fv(Cyan);
	glVertex3f(a, a, a);
	glVertex3f(-a, a, a);
	glVertex3f(-a, a, -a);
	glVertex3f(a, a, -a);
	glColor3fv(Green);
	glVertex3f(a, -2 * a, a);
	glVertex3f(a,  a, a);
	glVertex3f(a,  a, -a);
	glVertex3f(a, -2 * a, -a);
	glColor3fv(Cyan);
	glVertex3f(-a, -2 * a, a);
	glVertex3f(a, -2 * a, a);
	glVertex3f(a, -2 * a, -a);
	glVertex3f(-a, -2 * a, -a);
	glEnd();
	glDisable(GL_CULL_FACE);

	// rysowanie p�aszczyzny otworu w sze�cianie
	if (cutting_plane)
	{
		glDrawBuffer(GL_NONE); // wy��czenie rysowania w buforze kolor�w
		// rysowanie kwadratu cz�ciowo ods�aniaj�cego wn�trze sze�cianu (kwadrat jest po�o�ony o 0,001 jednostki nad bokiem sze�cianu)
		glBegin(GL_QUADS);
		glVertex3f( -0.5, -1.001, -0.5);
		glVertex3f( 0.5, -1.001, -0.5);
		glVertex3f( 0.5, -1.001, 0.5);
		glVertex3f( -0.5, -1.001, 0.5);
		glEnd();
		glDrawBuffer(GL_BACK); // w��czenie rysowania w buforze kolor�w
	}
	glColor3fv(Red); // w�a�ciwy sze�cian z obramowaniem, kt�rego rysowanie wymusza brak o�wietlenia
	if (polygon_offset)
		glEnable(GL_POLYGON_OFFSET_FILL);
	//glPolygonOffset(1.0, 1.0);
	drawSolidPyramid(N, 2.0);
	glColor3fv(Black);
	drawWirePyramid(N, 2.0);
	if (polygon_offset)
		glDisable(GL_POLYGON_OFFSET_FILL);
	glFlush(); // skierowanie polece� do wykonania
	glutSwapBuffers(); // zamiana bufor�w koloru
}

// zmiana wielko�ci okna
void Reshape(int width, int height)
{
	glViewport(0, 0, width, height); // obszar renderingu - ca�e okno
	glMatrixMode(GL_PROJECTION); // wyb�r macierzy rzutowania
	glLoadIdentity(); // macierz rzutowania = macierz jednostkowa
	glFrustum(left, right, bottom, top, near_, far_); // parametry bry�y obcinania
	Display(); // generowanie sceny 3D
}

// obs�uga klawiszy funkcyjnych i klawiszy kursora
void SpecialKeys(int key, int x, int y)
{
	switch (key)
	{
		// kursor w lewo
	case GLUT_KEY_LEFT:
		rotatey -= 1;
		break;
		// kursor w g�r�
	case GLUT_KEY_UP:
		rotatex -= 1;
		break;
		// kursor w prawo
	case GLUT_KEY_RIGHT:
		rotatey += 1;
		break;
		// kursor w d�
	case GLUT_KEY_DOWN:
		rotatex += 1;
		break;
	}
	
	Reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT)); // odrysowanie okna
}

// obs�uga przycisk�w myszki
void MouseButton(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		button_state = state; // zapami�tanie stanu lewego przycisku myszki
		// zapami�tanie po�o�enia kursora myszki
		if (state == GLUT_DOWN)
		{
			button_x = x;
			button_y = y;
		}
	}
}

// obs�uga ruchu kursora myszki
void MouseMotion(int x, int y)
{
	if (button_state == GLUT_DOWN)
	{
		rotatey += 30 * (right - left) / glutGet(GLUT_WINDOW_WIDTH) *(x - button_x);
		button_x = x;
		rotatex -= 30 * (top - bottom) / glutGet(GLUT_WINDOW_HEIGHT) *(button_y - y);
		button_y = y;
		glutPostRedisplay();
	}
}

// obs�uga menu podr�cznego
void Menu(int value)
{
	switch (value)
	{
		// p�aszczyzna przekroju
	case CUTTING_PLANE:
		cutting_plane = !cutting_plane;
		Display();
		break;
		// przesuni�cie warto�ci g��bi
	case POLYGON_OFFSET:
		polygon_offset = !polygon_offset;
		Display();
		break;
		// wyj�cie
	case EXIT:
		exit(0);
	}
}
int main(int argc, char * argv[])
{
	
	glutInit(&argc, argv); // inicjalizacja biblioteki GLUT
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); // inicjalizacja bufora ramki
	glutInitWindowSize(500, 500); // rozmiary g��wnego okna programu
	glutCreateWindow("Z-bufor"); // utworzenie g��wnego okna programu
	glutDisplayFunc(Display); // do��czenie funkcji generuj�cej scen� 3D
	glutReshapeFunc(Reshape); // do��czenie funkcji wywo�ywanej przy zmianie rozmiaru okna
	glutSpecialFunc(SpecialKeys); // do��czenie funkcji obs�ugi klawiszy funkcyjnych i klawiszy kursora
	glutMouseFunc(MouseButton); // obs�uga przycisk�w myszki
	glutMotionFunc(MouseMotion); // obs�uga ruchu kursora myszki
	glutCreateMenu(Menu); // utworzenie menu podr�cznego
	glutCreateMenu(Menu); // menu g��wne
	#ifdef WIN32
		glutAddMenuEntry("P�aszczyzna przekroju: rysowana/nierysowana", CUTTING_PLANE);
		glutAddMenuEntry("Przesuni�cie warto�ci g��bi: w��cz/wy��cz", POLYGON_OFFSET);
		glutAddMenuEntry("Wyj�cie", EXIT);
	#else
		glutAddMenuEntry("Plaszczyzna przekroju: rysowana/nierysowana", CUTTING_PLANE);
		glutAddMenuEntry("Przesuniecie wartosci glebi: wlacz/wylacz", POLYGON_OFFSET);
		glutAddMenuEntry("Wyjscie", EXIT);
	#endif

	glutAttachMenu(GLUT_RIGHT_BUTTON); // okre�lenie przycisku myszki obs�uguj�cej menu podr�czne
	glutIdleFunc(Display); // do��czenie funkcji wywo�ywanej podczas "bezczynno�ci" systemu
	glutMainLoop(); // wprowadzenie programu do obs�ugi p�tli komunikat�w
	return 0;
}