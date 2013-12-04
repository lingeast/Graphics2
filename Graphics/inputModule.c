/*
 * OpenGL demonstration program for ICS Computer Graphics courses
 * $Id: inputModule.c,v 1.2 2002/04/03 00:36:06 mshafae Exp $
 *
 */
#include <stdio.h>
#include <math.h>
#include <signal.h>
//#include <unistd.h>

#include "inputModule.h"
#include "myGl.h"

static int motionMode;
static int startX;  //position of mouse
static int startY;

static GLfloat viewX = 0; //position of view point
static GLfloat viewY = 10;
static GLfloat distance = 20.0;

static GLfloat angle = 20;    /* in degrees */
//static GLfloat angle2 = 30;   /* in degrees */
//static GLfloat xdistance = 0.0;
//static GLfloat ydistance = 0.0;
//static GLfloat zdistance = 0.0;

static GLfloat rotateAngle = 20; //angle of earth
static GLfloat radius = 15.0;//radius of earth

static GLfloat moonRadius = 7.0;
static GLfloat moonAngle = -20.0; //anlge of moon

static const float pi = 3.1415926;



void readKeyboard( unsigned char key, int x, int y ){
  switch( key ){
  case  0x1B: /* esc */
  case  'q':
//    kill( getpid(), SIGHUP );
    break;
  case ' ':

    break;
  case ',':

    break;
  case '.':

    break;
  case 'r':
    /* reset initial view parameters */
    angle = 20;
    viewX = 0;
	viewY = 10;
	distance = 20;
	rotateAngle = 20;
	moonAngle = -20;
    break;
  default:
    break;
  }
  glutPostRedisplay( );
}

void readSpecialKeys( int key, int x, int y ){
  switch( key ){
  case GLUT_KEY_UP:

    break;
  case GLUT_KEY_DOWN:

    break;
  case GLUT_KEY_RIGHT:

    break;
  case GLUT_KEY_LEFT:

    break;
  }
  glutPostRedisplay( );
}

void mouseButtHandler(int button, int state, int x, int y)
{
  motionMode = 0;

  switch( button ){
  case GLUT_LEFT_BUTTON:
    if( state == GLUT_DOWN ){
      /* Rotate object */
      motionMode = 1;
      startX = x;
      startY = y;
    }
    break;
  case GLUT_MIDDLE_BUTTON:
    if( state == GLUT_DOWN ){
      /* zoom in or zoom out */
      motionMode = 2;
      startX = x;
      startY = y;
    }
    break;
  case GLUT_RIGHT_BUTTON:
	  /*change x and y coordinate of the view point*/
    if( state == GLUT_DOWN ){
      /* Zoom */
      motionMode = 3;
      startX = x;
      startY = y;
    }
    break;
  }
  glutPostRedisplay( );
}

void mouseMoveHandler( int x, int y ){
  switch(motionMode){
  case 0:
    /* No mouse button is pressed... do nothing */
    /* return; */
    break;

  case 1:
    /* Calculate the rotations */
    angle = angle + (x - startX);
    //angle2 = angle2 + (y - startY);
	rotateAngle = rotateAngle + (x - startX);
	moonAngle = moonAngle + 5 * (x - startX);
    startX = x;
    startY = y;
    break;

  case 2:
    distance = distance - (x - startX) / 20.0;
    startX = x;
    startY = y;
    break;

  case 3:
    viewX = viewX - (x - startX) / 10.0;
	viewY = viewY - (y - startY) / 10.0;
    startX = x;
    startY = y;
    break;
  }

  glutPostRedisplay( );
}


//operation on the sun;
void setUserView( ){
  glLoadIdentity( );
  myGL_loadIdentity();

  //gluLookAt( viewX,viewY,distance,0.0,0.0,0.0,0.0,1.0,0.0 );
  myGL_lookAt(viewX, viewY, distance, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

  //basically no translation
  //glTranslatef( -xdistance, ydistance, -zdistance );

  //glRotatef( angle, 0.0, 1.0, 0.0 );
  myGL_rotated(angle, 0.0, 1.0, 0.0);
}

void setUserView_earth( ){
  //glLoadIdentity( );
  //gluLookAt( viewX,viewY,distance,0.0,0.0,0.0,0.0,1.0,0.0 );
  myGL_glMatrixMode(GL_MODELVIEW);
	myGL_loadIdentity();
  myGL_lookAt(viewX, viewY, distance, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

  //glScalef(0.5, 0.5, 0.5);
  myGL_scale(0.5, 0.5, 0.5);

  //translate the postion of the earth
  //glTranslatef( -radius * cos(rotateAngle * pi / 180) , 0.0, -radius * sin(rotateAngle * pi / 180) );
  myGL_translate(-radius * cos(rotateAngle * pi / 180), 0.0, -radius * sin(rotateAngle * pi / 180));

  //keep this scale and transition in stack, which will be used later in moon calcualtion
  //glPushMatrix();
  myGL_pushMatrix();
  //rotate, same speed as "sun"
  //glRotatef( angle, 0.0, 1.0, 0.0 );
  myGL_rotated(angle, 0.0, 1.0, 0.0);

  //glMultMatrixd(top());

}

void setUserView_moon( ) {
	//glLoadIdentity();
  //gluLookAt( viewX,viewY,distance,0.0,0.0,0.0,0.0,1.0,0.0 );
	myGL_lookAt(viewX, viewY, distance, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
  
  //translate earth in sun's coordinate
  //glPopMatrix();
	myGL_popMatrix();
  
  //glScalef(0.5, 0.5, 0.5);
	myGL_scale(0.5, 0.5, 0.5);
  
  //translate moon in earth's coordinate
  //glTranslatef(- moonRadius * cos(moonAngle * pi / 180) , 0.0, - moonRadius * sin((moonAngle * pi / 180)) );
	myGL_translate(-moonRadius * cos(moonAngle * pi / 180), 0.0, -moonRadius * sin((moonAngle * pi / 180)));
  
  //rotate, same speed as "sun"
  //glRotatef( angle, 0.0, 1.0, 0.0 );
	myGL_rotated(angle, 0.0, 1.0, 0.0);

	//glMultMatrixd(top());
}


void TimerFunction(int value) {
	//earth's rotate angle increases by 1 degree every 33ms
	rotateAngle += 1;

	if (rotateAngle > 360)
		rotateAngle -= 360;
	
	angle += 1;
	if (angle > 360)
		angle -= 360;

	moonAngle -= 3;
	if (moonAngle < -360)
		angle += 360;

	glutPostRedisplay();
	
	//glut time function
	glutTimerFunc(33, TimerFunction, 1);
}
