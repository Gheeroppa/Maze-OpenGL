#include <stdlib.h>
#include <cmath>
#include <GL/glut.h> 

using namespace std; 

void DisegnaPavimento(GLfloat R)
{
	glBegin(GL_QUADS);
	
	glNormal3f(0, 1, 0);
	glTexCoord2f(1, 1);
	glVertex3f(R, R, -R);
	glTexCoord2f(0, 1);
	glVertex3f(-R, R, -R);
	glTexCoord2f(0, 0);
	glVertex3f(-R, R, R);
	glTexCoord2f(1, 0);
	glVertex3f(R, R, R);
		
	glEnd();
}

void DisegnaAllarme(GLfloat R)
{
  glColor3f(1.0,0.0,0.0);
  glutSolidCube(R);
}

void DisegnaMuro(GLfloat R)
{
	glBegin(GL_QUADS);
	
	glNormal3f(0, 0, 1);
	glTexCoord2f(1, 1);
	glVertex3f(R, R, R);
	glTexCoord2f(0, 1);
	glVertex3f(-R, R, R);
	glTexCoord2f(0, 0);
	glVertex3f(-R, -R, R);
	glTexCoord2f(1, 0);
	glVertex3f(R, -R, R);
	
	glNormal3f(0, 0, -1);
	glTexCoord2f(1, 1);
	glVertex3f(-R, R, -R);
	glTexCoord2f(0, 1);
	glVertex3f(R, R, -R);
	glTexCoord2f(0, 0);
	glVertex3f(R, -R, -R);
	glTexCoord2f(1, 0);
	glVertex3f(-R, -R, -R);
	
	glNormal3f(1, 0, 0);
	glTexCoord2f(1, 1);
	glVertex3f(R, R, -R);
	glTexCoord2f(0, 1);
	glVertex3f(R, R, R);
	glTexCoord2f(0, 0);
	glVertex3f(R, -R, R);
	glTexCoord2f(1, 0);
	glVertex3f(R, -R, -R);
	
	glNormal3f(-1, 0, 0);
	glTexCoord2f(1, 1);
	glVertex3f(-R, R, R);
	glTexCoord2f(0, 1);
	glVertex3f(-R, R, -R);
	glTexCoord2f(0, 0);
	glVertex3f(-R, -R, -R);
	glTexCoord2f(1, 0);
	glVertex3f(-R, -R, R);
	
	glNormal3f(0, 1, 0);
	glTexCoord2f(1, 1);
	glVertex3f(R, R, -R);
	glTexCoord2f(0, 1);
	glVertex3f(-R, R, -R);
	glTexCoord2f(0, 0);
	glVertex3f(-R, R, R);
	glTexCoord2f(1, 0);
	glVertex3f(R, R, R);
	
	glNormal3f(0, -1, 0);
	glTexCoord2f(1, 1);
	glVertex3f(-R, -R, -R);
	glTexCoord2f(0, 1);
	glVertex3f(R, -R, -R);
	glTexCoord2f(0, 0);
	glVertex3f(R, -R, R);
	glTexCoord2f(1, 0);
	glVertex3f(-R, -R, R);
	
	glEnd();
}

void Draw_Skybox(float x, float y, float z, float width, float height, float length)
{
  	// Reset and transform the matrix.
	//glLoadIdentity(); 
	
	// Store the current matrix
	glPushMatrix();
    	
	// Enable/Disable features
	/*glPushAttrib(GL_ENABLE_BIT);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_BLEND);*/
    
	// Center the Skybox around the given x,y,z position
	x = x - width  / 2;
	y = y - height / 2;
	z = z - length / 2;


	// Draw Front side
	glBindTexture(GL_TEXTURE_2D, 4);
	glBegin(GL_QUADS);	
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x, 	y, 		z+length);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x, 	y+height,	z+length);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x+width, 	y+height, 	z+length); 
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x+width, 	y, 		z+length);
	glEnd();

	// Draw Back side
	glBindTexture(GL_TEXTURE_2D, 4);
	glBegin(GL_QUADS);		
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x+width, 	y, 		z);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x+width, 	y+height, 	z); 
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x,		y+height, 	z);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x, 	y, 		z);
	glEnd();

	// Draw Left side
	glBindTexture(GL_TEXTURE_2D, 4);
	glBegin(GL_QUADS);		
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x,		  y+height,	z);	
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x,		  y+height,	z+length); 
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x,		  y,		z+length);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x,		  y,		z);		
	glEnd();

	// Draw Right side
	glBindTexture(GL_TEXTURE_2D, 4);
	glBegin(GL_QUADS);		
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x+width,	  y,		z);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x+width, 	  y,		z+length);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x+width, 	  y+height,	z+length); 
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x+width, 	  y+height,	z);
	glEnd();

	// Draw Up side
	glBindTexture(GL_TEXTURE_2D, 4);
	glBegin(GL_QUADS);		
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x+width, 	  y+height, 	z);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x+width, 	  y+height, 	z+length); 
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x,		  y+height,	z+length);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x,		  y+height,	z);
	glEnd();
	
	// Restore enable bits and matrix
	//glPopAttrib();
	glPopMatrix();
}