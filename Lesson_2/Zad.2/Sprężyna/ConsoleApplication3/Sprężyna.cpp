#include "stdafx.h" //biblioteka pozwalajaca przyspieszenie kompilacji programu C
#include "math.h" //biblioteka funkcji matematycznej
//Je�li zdefiniowane na komputer APPLE
//{
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h> //specyfikacja otwartego i uniwersalnego API do tworzenia grafiki
#endif
//}

#define GL_PI 3.1415f //zdefiniowany typ PI

//Wielko�� obrot�w
static GLfloat xRot = 0.0f; //pocz�tkowy obr�t wzgl�dem osi x
static GLfloat yRot = 0.0f; //pocz�tkowy obr�t wzgl�dem osi y

GLfloat x, y, z, angle;

//Wywo�ywana w celu przerysowania sceny
void RenderScene(void)
{
	//Zmienne przechowujce wsp�rzdne i kty  x11
	glClear(GL_COLOR_BUFFER_BIT); //Wyczyszczenie okna aktualnym kolorem czyszcz�cym

	//Zapisanie stanu macierzy i wykonanie obrotu
	glPushMatrix();
	glRotatef(xRot, 1.0f, 0.0f, 0.0f);
	glRotatef(yRot, 0.0f, 1.0f, 0.0f);

	//Wywo�a tylko raz, przed rysowaniem wszystkich punkt�w
	glBegin(GL_POINTS); //Rysuje punkty na ekranie. Ka�dy okre�lony wierzcho�ek jest punktem.
	x = -50.0f;

	GLfloat sizes[2]; //Do zapisania dopuszczalnego zakresu wielko�ci
	GLfloat step; //Zapami�tuje obs�ugiwany przyrost rozmiaru
	
	//Pobranie dopuszczalnych rozmiar�w punkt�w
	glGetFloatv(GL_POINT_SIZE_RANGE, sizes);
	glGetFloatv(GL_POINT_SIZE_GRANULARITY, &step);
	GLfloat curSize = sizes[0];
	for (angle = 0.0f; angle <= (2.0f*GL_PI)*8.0f; angle += 0.1f)
	{
		z = 50.0f*sin(angle); //z i y okre�la po�o�enie punktu wzgl�dem ich osi
		y = 50.0f*cos(angle);

		//Okrelenie punktu i przesuni�cie wsp�rz�dnej Z
		glVertex3f(x, y, z);
		x += 0.52f;
		glPointSize(curSize);
		curSize += step;

	}
	
	glEnd(); //Zakoczenie rysowania punkt�w
	glPopMatrix(); //Odtworzenie macierzy przekszta�ce
	glutSwapBuffers(); //Wykonanie polecenia rysowania
}

//Ta funkcja wykonuje wszystkie konieczne inicjalizacje kontekstu renderowania
void SetupRC()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //Czarne t�o
	glColor3f(1.0f, 2.0f, 3.0f); //B�dziemy rysowa� kolorem zielonym
}

//sterowanie klawiszami
void SpecialKeys(int key, int x, int y)
{
	if (key == GLUT_KEY_UP)
		xRot -= 5.0f;
	if (key == GLUT_KEY_DOWN)
		xRot += 5.0f;
	if (key == GLUT_KEY_LEFT)
		yRot -= 5.0f;
	if (key == GLUT_KEY_RIGHT)
		yRot += 5.0f;
	if (key > 356.0f)
		xRot = 0.0f;
	if (key < -1.0f)
		xRot = 355.0f;
	if (key > 356.0f)
		yRot = 0.0f;
	if (key < -1.0f)
		yRot = 355.0f;

	glutPostRedisplay(); //Odwie�enie zawartoci okna
}

//zmiana rozmiaru okna
void ChangeSize(int w, int h)
{
	GLfloat nRange = 100.0f;

	//Zabezpieczenie przed dzieleniem przez zero
	if (h == 0)
		h = 1;
	
	glViewport(0, 0, w, h); //Ustalenie wymiar�w widoku na zgodnych z wymiarami okna
	glMatrixMode(GL_PROJECTION); //Ponowne ustawienie stosu macierzy rzutowania
	glLoadIdentity();

	//Utworzenie przestrzeni ograniczajcej (lewo, prawo, d�, g�ra, blisko, daleko)
	if (w <= h)
		glOrtho(-nRange, nRange, -nRange*h / w, nRange*h / w, -nRange, nRange);
	else
		glOrtho(-nRange*w / h, nRange*w / h, -nRange, nRange, -nRange, nRange);

	//Ponowne ustawienie stosu macierzy rzutowania
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("Points Example");
	glutReshapeFunc(ChangeSize);
	glutSpecialFunc(SpecialKeys);
	glutDisplayFunc(RenderScene);
	SetupRC();
	glutMainLoop();
	return 0;
}
