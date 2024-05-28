// #include <glut.h>
// #include <glaux.h>
//#include <time.h>
#include "gl.h"
#include <windows.h>
#include <math.h>
double x10 = 10, x20 = 2;
double dt=0.001, t=0;
bool autorun=false;
int k_t=1000;
GLint Width=16*40, Height=9*40;

struct cur_x{
	double x1,x2,n1,n2,u;
	double x1p,x2p,n1p,n2p,up;
	cur_x(){
		x1=n1=x10; x2=x20; u=n2=0;
		x1p=x2p=n1p=n2p=up=0;
	}
	void next_moment(){
        t+=dt;
		x1p=x1 +x2*dt;
		x2p=x2 +(4.9*x1-0.25*u)*dt;
		up=819.6*n1+408*n2; //matrix k is inputing
		n1p=n1 +(102*(x1-n1)+n2)*dt; //matrix l is inputing
		n2p=n2 +(-200*n1+204.9*x1-0.25*u)*dt;
		x1=x1p; x2=x2p; n1=n1p; n2=n2p; u=up;
	}
	void plus_t(double t){
        for (double i=0;i<t/dt;i++)
            next_moment();
    }
	void maket0(){
		x1=n1=x10; x2=x20; u=n2=0;
		x1p=x2p=n1p=n2p=up=0;
	}
};
cur_x obj;

void drawCircle(double radius, double x, double y) {
    double t = .0;
        glBegin(GL_TRIANGLE_FAN);
        glVertex2d(x, y);
        for(t = 0; t <= 2 * 3.1415; t += 2 * 3.1415 / 100.0) {
            glVertex2d(x+radius*cos(t),y+radius*sin(t));
        }
        glEnd();
}

void Draw_line(GLint x, GLint y, GLint t){
     glColor3ub(0, 0, 255);
     int a=Width/12/10;
     int b=Height/3;
    //may increase theta n times for more observability
	glPushMatrix();
	glTranslatef(x, y, -0);
    glPushMatrix();
    glRotated(t*1, 0, 0, 1.0);
	glTranslatef(-x, -y, -0);
    glBegin(GL_QUADS);
                          glVertex2f(x-a/2,y-b/50);
                          glVertex2f(x+a/2,y-b/50);
                          glVertex2f(x+a/2,y+0.98*b);
                          glVertex2f(x-a/2,y+0.98*b);
    glEnd();
    glColor3ub(255,255,255);
    drawCircle(1.5*a,x,y+0.98*b);
    glPopMatrix();
    glPopMatrix();
}
void Draw_triangle(GLint x, GLint y){
	glColor3ub(200,103,6);
	double a=Width/20;
	double k=0.57735;
     glBegin(GL_TRIANGLES);
		glVertex2f(x, y+k*a);
		glVertex2f(x-a/2,y-a*k/2);
		glVertex2f(x+a/2,y-a*k/2);
     glEnd();
}
void Draw(void){
    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0,0,0,1); //background
	Draw_line(Width/2,Height/4,-obj.n1);
	Draw_triangle(Width/2,Height/4);
	glFlush();
}
void Reshape(GLint w, GLint h){
     Width=w;
     Height=h;
     glViewport(0,0,w,h);
     glMatrixMode(GL_PROJECTION);
     glLoadIdentity();
     glOrtho(0,w,0,h,-1.0,1.0);
     glMatrixMode(GL_MODELVIEW);
     glLoadIdentity();
}
void Keyboard( unsigned char key, int x, int y){
    const char ESCAPE = '\033';
	const char AUTORUN = 'a';
	switch( key ) {
		case ESCAPE:
			exit(0);
			break;
		case AUTORUN:
			autorun = !autorun;
			break;
		case 'q':
			obj.maket0();
			t=0;
			break;
		case 's':
			k_t=1000-k_t;
			break;
		case 32:
			for (int i=0;i<5;i++)
			obj.next_moment();
		}
	glutPostRedisplay();
}
void IdleFunc(){
	     if (autorun){
                  t+=dt;
                  obj.next_moment();
                  glutPostRedisplay();
                  Sleep(k_t*dt);
                  }
}

int main(int argc, char *argv[])
{
  glutInit(&argc, argv);
  glutInitWindowSize(Width, Height);
  glutInitWindowPosition(100, 100);
  glutInitDisplayMode(GLUT_RGB);
  glutCreateWindow("Stable me. Press a/q/s");
  glClearColor(0, 0, 0, 0);

  glutDisplayFunc(Draw);
  glutReshapeFunc(Reshape);
  glutKeyboardFunc(Keyboard);
  glutIdleFunc(IdleFunc);

  glutMainLoop();
  return 0;
}
