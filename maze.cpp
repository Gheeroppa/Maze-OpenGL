//=============================================================================
// maze.cpp
//
// Federico Ghirardelli - UniPD
//=============================================================================

#include <stdlib.h>
#include <cmath>
#include <GL/glut.h>
#include <time.h>  
#include <stdio.h>
#include <string.h>
#include <string>
#include <sstream>

namespace patch
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }
}

#include <iostream>

#include "draw.cpp"
#include "texture.cpp"
#include "audio.cpp"

using namespace std;

//----CONSTANTS------
#define GRID_WIDTH 15 // !! DISPARI !!
#define GRID_HEIGHT 15
#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3

static double yVal=1.0;
//--------------------------------------------------
GLint ScreenWidth = 800, ScreenHeight = 600;

const GLfloat DEG2RAD = 3.14159/180.0;

int maze[GRID_WIDTH*GRID_HEIGHT];
int alarm[GRID_WIDTH*GRID_HEIGHT];
int wall[GRID_WIDTH][GRID_HEIGHT];

int numAlarmTot = 0;
int numAlarmPresi = 0;
int score = 0;
//--------------------------------------------------
GLfloat pos[3]	= {1.0, 0.1, 1.0};
GLfloat at[3]	= {0.0, 0.1, 0.0};
GLfloat up[3]	= {0.0, 1.0, 0.0};

/* Vettore direzione dello sguardo */
GLfloat dir[3]	= {0.0, 0.0, -0.1};

/* titolo finestra */

char const* comandi = "(W/A/S/D - SPACE)    Gold: ";
char titolo[50];
std::string strPresi;
std::string strTotali;
std::string strScore;

//--------------------------------------------------
int TIME = 50000;

//-- FUNCTION PROTOTYPES ---------------------------
void display(void);
void init(void);
void idle(void);
void myReshape(int width,int height);
void MyMouse(int button,int state,int x,int y);
void MyKeyBoard(unsigned char key,int x,int y);
void rotateY(float, float, float, float);
void cross(float, float, float);

GLuint loadBMP_custom(const char * imagepath);
void caricaTexture(void);

void initMaze();
void initAlarm();
int XYToIndex( int x, int y );
int IsInBounds( int x, int y );
void Visit( int x, int y );

void DisegnaMuro(GLfloat);
void DisegnaPavimento(GLfloat);
void Draw_Skybox(float x, float y, float z, float width, float height, float length);
void DisegnaAllarme(GLfloat);

int checkMovement(int direction);
void checkAlarm();

void drawText(const char *text, int length, float x, float y);
void scoring();
//------------------------------------------------------------------------------------------------------------------------------

int main(int argc, char **argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); 
  
  glutInitWindowPosition(0,0);
  glutInitWindowSize(ScreenWidth,ScreenHeight);
  
  glutCreateWindow(comandi);
 
  caricaTexture();
	
  { // spotlight
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    // spotLight values ambient
    GLfloat mat_specular[] = { 0.0, 0.0, 0.0, 0.0 };
    GLfloat mat_shininess[] = { 1.0 };
    GLfloat light_position[] = { 0.0,0.0,6.0, 1.0 };
    GLfloat spotDir[] = { 0.0,0.0,-1.0 };

    GLfloat light_ambient[] = { 0.1, 0.1, 0.2, 0.0};
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    
    glShadeModel(GL_SMOOTH);
    
    //glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 2.0);
    //glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION,0.4);
    //glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.5);
    glLightfv(GL_LIGHT0,GL_SPECULAR,mat_specular);
    glLightfv(GL_LIGHT0,GL_POSITION,light_position);
    // Definig spotlight attributes
    glLightf(GL_LIGHT0,GL_SPOT_CUTOFF,7.0); // angolo di apertura cono
    glLightf(GL_LIGHT0,GL_SPOT_EXPONENT,128); //  0 - 128
    glLightfv(GL_LIGHT0,GL_SPOT_DIRECTION,spotDir);
  }
    
  srand( time(0) );
  initMaze();  
  Visit(1,1);
  initAlarm();
  
  
  glutDisplayFunc(display);
  glutIdleFunc(idle);
  glutReshapeFunc(myReshape);
  //glutMouseFunc(MyMouse);
  glutKeyboardFunc(MyKeyBoard);
  
  init();
 
  initSound();
  
  strcpy(titolo,comandi);
  const char * c1 = strPresi.c_str();
  const char * c2 = strTotali.c_str();
  const char * c3 = strScore.c_str();
  strcat(titolo,c1);
  strcat(titolo," / ");
  strcat(titolo,c2);
  strcat(titolo,"  -  SCORE: ");
  strcat(titolo,c3);
  
  glutMainLoop();
  
  return 0;
}
//************************************************************************************************************************ main

void init()
{
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glClearColor (0.0, 0.0, 0.0, 0.0);
  
  glLoadIdentity();
}

void idle()
{
  if(TIME>1)
  {
    TIME = TIME - 1;
  }
  glutPostRedisplay();
}

void display()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    
    
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity ();
  
  glutSetWindowTitle(titolo);
  
    /* posiziono la camera */
  glPushMatrix();
  gluLookAt(pos[0], pos[1], pos[2], 
	    at[0],  at[1],  at[2], 
	    up[0],  up[1],  up[2]);
  //+++++++++++++++++++++++++++++++++++++++++++++++++++
  
  GLfloat ambiente[4] = { 1, 1, 1, 1 };
  GLfloat direttiva[4] = { 1, 1, 1, 1 };

  GLfloat brillante_1[4] = { 1, 1, 1, 1 };
  GLfloat brillante_2[4] = { 0, 0, 0, 1 };
	
  glMateriali(GL_FRONT, GL_SHININESS, 32);
  glMaterialfv(GL_FRONT, GL_AMBIENT, ambiente);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, direttiva);
  
  glEnable(GL_TEXTURE_2D);
  
  // VARIABILI //
   int i,j;
   float x,y,z;
	
  Draw_Skybox(0,0,0,300,300,300);
    
  
	for(i=0;i<GRID_WIDTH;i++)
	{
		for(j=0;j<GRID_HEIGHT;j++)
		{
			
			if(maze[XYToIndex(i,j)] != 0) // muro
			{
				glPushMatrix();
				  x = i;  
				  z = j;
				  glTranslatef(x, 0, z);
				  glMaterialfv(GL_FRONT, GL_SPECULAR, brillante_2);
				  glBindTexture(GL_TEXTURE_2D, 2);
				  //glutSolidCube(1.0);
				  DisegnaMuro(0.5);
				glPopMatrix();
			}
			else			// pavimento
			{
				glPushMatrix();
				  x = i;  
				  z = j;
				  glTranslatef(x, -1, z);
				  glMaterialfv(GL_FRONT, GL_SPECULAR, brillante_1);
				  glBindTexture(GL_TEXTURE_2D, 1);
				  DisegnaPavimento(0.5);
				  //glutSolidCube(1.0);
				glPopMatrix();
				
				if(alarm[ XYToIndex(i,j) ]==1)  // cubo dorato
				{
				  glPushMatrix();
				    x = i;  
				    z = j;
				    glTranslatef(x, -0.2, z);
				    glMaterialfv(GL_FRONT, GL_SPECULAR, brillante_2);
				    glBindTexture(GL_TEXTURE_2D, 3);
				    DisegnaMuro(0.1);
				  glPopMatrix();
				}
				wall[i][j] = 0; 
				
			}
			
		}
	}
  glPopMatrix();
  
   // GUI  (rimossa perchè rallenta) 
  
   /*glPushMatrix();   // disegna ALARMS rimasti/totali
   
      std::ostringstream oss;
      std::string text = "ALARMS ";
      std::string text2 = " / ";
      oss << text << numAlarmPresi << text2 << numAlarmTot;
      std::string stringa = oss.str();
      //string String = static_cast<ostringstream*>( &(ostringstream() << Number) )->str();
      drawText(stringa.data(), stringa.size(), ScreenWidth-150, ScreenHeight-50);
      
    glPopMatrix();*/
  //++++++++++++++++ 
  glutSwapBuffers();
  glFlush();
}

void myReshape(int width,int height)
{
    glViewport (0, 0, (GLsizei) width, (GLsizei) height);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    
    gluPerspective(90.0, 1.0, 0.1, 300.0);
    //glFrustum (-1.0, 1.0, -1.0, 1.0, 1.5, 20.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void drawText(const char *text, int length, float x, float y)
{
 glMatrixMode(GL_PROJECTION); // change the current matrix to PROJECTION
 double *matrix = new double[16]; // 16 doubles in stack memory
 glGetDoublev(GL_PROJECTION_MATRIX, matrix); // get the values from PROJECTION matrix to local variable
 glLoadIdentity(); // reset PROJECTION matrix to identity matrix
 glOrtho(0, 800, 0, 600, -5, 5); // orthographic perspective
 glMatrixMode(GL_MODELVIEW); // change current matrix to MODELVIEW matrix again
 glLoadIdentity(); // reset it to identity matrix
 glPushMatrix(); // push current state of MODELVIEW matrix to stack
 glLoadIdentity(); // reset it again. (may not be required, but it my convention)
 glRasterPos2i(x, y); // raster position in 2D
 for(int i=0; i<length; i++){
  glutBitmapCharacter(GLUT_BITMAP_9_BY_15, (int)text[i]); // generation of characters in our text with 9 by 15 GLU font
 }
 glPopMatrix(); // get MODELVIEW matrix value from stack
 glMatrixMode(GL_PROJECTION); // change current matrix mode to PROJECTION
 glLoadMatrixd(matrix); // reset
 glMatrixMode(GL_MODELVIEW); // change current matrix mode to MODELVIEW
}

void initMaze()
{
  // inizializzazione matrici labirinto/allarmi
  for (int i=0; i<GRID_WIDTH*GRID_HEIGHT; ++i)
  {
    maze[i] = 1;
    alarm[i]= 0;
  } 
  
  for (int i=0; i<GRID_WIDTH; i++)
  {
    for(int j=0; j<GRID_HEIGHT; j++)
    {
      wall[i][j] = 1;
    }
  } 
}

void initAlarm() // inserimento cubi dorati
{
  int aa = 0;
  int numC = GRID_WIDTH * GRID_HEIGHT;
  int nn = (int) sqrt(numC)/2;
  
  for (int i=0; i<GRID_WIDTH; i++)
  {
    for(int j=0; j<GRID_HEIGHT; j++)
    {
      if(maze[ XYToIndex(i,j) ]==0)
      {
	aa = rand() % nn ;
	if(aa == 1)
	{
	  numAlarmTot++;
	  alarm[ XYToIndex(i,j) ] = 1;
	}
      }
      
    }
  } 
  numAlarmPresi = 0;
  
  strPresi = patch::to_string(numAlarmPresi);
  strTotali = patch::to_string(numAlarmTot);
  strScore = patch::to_string(score);

}

int XYToIndex( int x, int y )
{
    // Converts the two-dimensional index pair (x,y) into a
    // single-dimensional index. The result is y * ROW_WIDTH + x.
    return y * GRID_WIDTH + x;
}

int IsInBounds( int x, int y )
{
    // Returns "true" if x and y are both in-bounds.
    if (x < 0 || x >= GRID_WIDTH) return false;
    if (y < 0 || y >= GRID_HEIGHT) return false;
    return true;
}

void Visit( int x, int y )
{
  // Starting at the given index, recursively visits every direction in a randomized order.
  // Set my current location to be an empty passage.
  maze[ XYToIndex(x,y) ] = 0;
  
  // Create an local array containing the 4 directions and shuffle their order.
  int dirs[4];
  dirs[0] = NORTH;
  dirs[1] = EAST;
  dirs[2] = SOUTH;
  dirs[3] = WEST;
  
  for (int i=0; i<4; ++i)
  {
    int r = rand() % 3;
    int temp = dirs[r];
    dirs[r] = dirs[i];
    dirs[i] = temp;
  }
  
  // Loop through every direction and attempt to Visit that direction.
  for (int i=0; i<4; ++i)
  {
    // dx,dy are offsets from current location. Set them based on the next direction I wish to try.
    int dx=0, dy=0;
    switch (dirs[i])
    {
    case NORTH: dy = -1; break;
    case SOUTH: dy = 1; break;
    case EAST: dx = 1; break;
    case WEST: dx = -1; break;
    }
    
    // Find the (x,y) coordinates of the grid cell 2 spots
    // away in the given direction.
    int x2 = x + (dx<<1);
    int y2 = y + (dy<<1);
    if (IsInBounds(x2,y2))
    {
      if (maze[ XYToIndex(x2,y2) ] == 1)
      {
	// (x2,y2) has not been visited yet... knock down the
	// wall between my current position and that position
	maze[ XYToIndex(x2-dx,y2-dy) ] = 0;
	// Recursively Visit (x2,y2)
	Visit(x2,y2);
      }
     }
   }
}

/* Ruota un vettore intorno all'asse delle Y */
void rotateY(float *x, float *y, float *z, float angle)
{
	float xs = *x;
	float	ys = *y;
	float	zs = *z;
	float theta = angle * DEG2RAD;  
	*x = (xs * cos(theta)) + (zs * sin(theta));
	*y = ys;
	*z = (xs * -sin(theta)) + (zs * cos(theta));
}

int checkMovement(int direction)
{
  int go = 0;
  int xx = (int) (pos[0]+0.5), zz = (int) (pos[2]+0.5); //dove mi trovo
  int xx2 = 0, zz2 = 0; //la casella in cui finisco
  
  if(direction == 1) // avanti
  {
      xx2 = (int) (pos[0]+0.5 +dir[0]+(1*dir[0])*3);
      zz2 = (int) (pos[2]+0.5 +dir[2]+(1*dir[2])*3);
      
  }
  else //indietro
  {
      xx2 = (int) (pos[0]+0.5 - dir[0]+(1*dir[0])*3);
      zz2 = (int) (pos[2]+0.5 - dir[2]+(1*dir[2])*3);
  }
    
  if(wall[xx2][zz2] == 0)
  {
    go = 1;
  }
  else
  {
    go = 0;
  }
  
  return go;
}

void checkAlarm() 
{
  /* rilevo se sono in una casella con un cubo dorato
   * in caso positivo emetto il suono
   */
  
  int xx = 0;
  xx = (int) (pos[0]+0.5);
  
  int zz = 0;
  zz = (int) (pos[2]+0.5); //dove mi trovo

    if(alarm[ XYToIndex(xx,zz) ] == 1)
    {
      alarm[ XYToIndex(xx,zz) ] = 0;
      numAlarmPresi++;
      playSound();
      scoring(); // calcolo il punteggio
      
      // aggiorno la stringa del titolo
      strPresi = patch::to_string(numAlarmPresi);
      strScore = patch::to_string(score);
    

	strcpy(titolo,comandi);
	const char * c1 = strPresi.c_str();
	const char * c2 = strTotali.c_str();
	const char * c3 = strScore.c_str();
	strcat(titolo,c1);
	strcat(titolo," / ");
	strcat(titolo,c2);
	strcat(titolo,"  -  SCORE: ");
	strcat(titolo,c3);
	
      if(numAlarmPresi==numAlarmTot) // se non li ho ancora presi tutti
      {
	strcat(titolo,"     YOU WIN ! "); //messaggio di fine gioco
      }
    }
}

void scoring()
{
  score = score + (TIME / 1000);
}
//------------ LISTENER  -----------------------------------------------------------------------------------------------------------

void MyKeyBoard(unsigned char key, int x, int y)
{
	switch ( key ) {
			case 27:  /*  Escape key  */
				exit ( 0 );
				break;
			case 'w': /* Spostati in avanti */
				if(checkMovement(1) == 1)
				{
				  pos[0] += dir[0];
				  pos[1] += dir[1];
				  pos[2] += dir[2];
				  at[0] = pos[0] + dir[0];
				  at[1] = pos[1] + dir[1];
				  at[2] = pos[2] + dir[2];
				}
				break;
			case 's': /* Spostati all'indietro */
			        if(checkMovement(2) == 1)
				{
				  pos[0] -= dir[0];
				  pos[1] -= dir[1];
				  pos[2] -= dir[2];
				  at[0] = pos[0] + dir[0];
				  at[1] = pos[1] + dir[1];
				  at[2] = pos[2] + dir[2];
				}
				break;
			case 'd': /* Ruota la camera a sinistra */
				rotateY(&dir[0], &dir[1], &dir[2], -2.0);
				at[0] = pos[0] + dir[0];
				at[1] = pos[1] + dir[1];
				at[2] = pos[2] + dir[2];
				break;
			case 'a': /* Ruota la camera a destra */
				rotateY(&dir[0], &dir[1], &dir[2], 2.0);
				at[0] = pos[0] + dir[0];
				at[1] = pos[1] + dir[1];
				at[2] = pos[2] + dir[2];
				break;
			case 'u': /* up */
				pos[1] += 1;
				break;
			case ' ': /* togli allarme */
				checkAlarm();
				break;
			default:
				break;
	}
}
//------------------------------------------------------------------------------------------------------------------------- 
