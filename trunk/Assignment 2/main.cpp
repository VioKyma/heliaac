// Assignment 2.cpp
// Heliaac
// Christopher Trott, Ashley Sexton, Aleesha Torkington
// Created on 28/08/09
// Last Modified on 13/10/09 @ 13:08
//
// Some of this code is taken from animlightpos.cpp on the LearnJCU resources page
// Some code to do with lighting was gained from the URL: http://www.falloutsoftware.com/tutorials/gl/gl8.htm
// Some code to do with text on screen gained from Lighthouse 3D @ URL: http://www.lighthouse3d.com/opengl/glut/index.php?bmpfontortho
// Bitmap.h and Bitmap.cpp found at: http://www.gamedev.net/reference/articles/article1966.asp

#include<GL/freeglut.h>
#include<math.h>
#include<string>
#include<iostream>
#include"Bitmap.h"

using namespace std;

struct vertex
{
    float x;
    float y;
    float z;
};

struct objectBox
{
    float xPos;        // x position
    float yPos;        // y position
    float zPos;        // z position
	float rotX;     // rotation angle in x axis
    float rotY;     // rotation angle of direction (y axis)
	float rotZ;		// rotation angle in z axis
    float xSize;     // x radius (half-length) of bounding box
    float ySize;     // y radius of bounding box
    float zSize;     // z radius of bounding box
};

struct checkPoint
{
	int checkpoint;
	float xSize;
	float ySize;
	float zSize;
	float xPos;
	float yPos;
	float zPos;
	float rotY;
	bool activated;
};

void drawHeli(void);
void drawHeliBody(void);
void drawHeliRotor(void);
void drawGround(void);
void moveHeliForward(float speed, bool checkCol);
void moveHeliBack(float speed, bool checkCol);
void moveHeliDown(float speed, bool checkCol);
void moveHeliUp(float speed, bool checkCol);
void checkBounds(void);
bool checkBoxCollision(objectBox object1, objectBox object2);
void checkHeliCollisions(void);
void drawBuilding(void);
void updateFPS(void);
void updateGameTime(void);
void displayText(void);
void resetPerspectiveProjection(void);
void setOrthographicProjection(void);
void renderBitmapString(float x, float y, void *font,char *string);
void displayHelp(void);
float cosDeg(float degRot);
float sinDeg(float degRot);
GLuint loadTextureBMP(char * filename, int wrap, int width, int height);

float cameraDistance = 5.0;
objectBox heli = {0, 2, 0, 0, 0, 0, 2, 2, 1};
float windscreenRot = 0.0;

objectBox eye = {cameraDistance, heli.yPos, cameraDistance, 0, 135, 0, 0, 0, 0};
objectBox building0 = {10, 5, 10, 0, 0, 0, 2, 5, 2};

bool movingForward = false;
bool movingBack = false;
bool movingUp = false;
bool movingDown = false;
bool turningLeft = false;
bool turningRight = false;

int font = (int)GLUT_BITMAP_HELVETICA_18;
int textX = 20;
int textY = 20;

bool light0 = true;
bool light1 = false;

//trying to implement start/stop without changing functions too much
bool helicopterOn = false;

GLfloat light0_position[] = { 1, 20, 1, 0 };

GLuint heliBodyList;
GLuint heliRotorList;
GLuint groundList;

double rotor = 0;
const double MAX_ROTOR_SPEED = 10;
double rotorSpeed = 0.0;
int groundSize = 20;
int groundHeight = 0;

int windowWidth = 500;
int windowHeight = 500;

int windowPosWidth = 100;
int windowPosHeight = 100;

int frames = 0;
int time = 0;
int timeBase = 0;
float fps = 50.0;
char* strFps = new char[4];     // FPS string for display on-screen
char* strGameTime = new char[8];	// Game time string
int bestTime = 0;
char* strBestTime = new char[8];
int gameTime = 0;
int gameTimeBase = 0;

const int ROTATE_SPEED = 180;
const int HELI_SPEED = 12;
const float LEAN_FACTOR = 15.0;
float heliLeanFront = 0.0;
float heliLeanSide = 0.0;
int rotSpeed = ROTATE_SPEED / fps;
float heliSpeed = HELI_SPEED / fps;

bool pause = false;
bool wire = false;

int last_mouse_x = 0;
int last_mouse_y = 0;
bool leftMouseDown = false;

float pi = 3.1415926535897932384626433832795;

GLuint textures[10];

const int MAX_CHECKPOINTS = 10;
checkPoint points[MAX_CHECKPOINTS];

// Initialise the OpenGL properties
void init(void)
{
    glEnable(GL_DEPTH_TEST);
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
    glShadeModel(GL_FLAT);

    // Make object materials equal to glColor3f() properties
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

    // Define the heliBody display list
    heliBodyList = glGenLists(1);
    glNewList(heliBodyList, GL_COMPILE);
    drawHeliBody();
    glEndList();
   
    // Define the heliRotor display list
    heliRotorList = glGenLists(1);
    glNewList(heliRotorList, GL_COMPILE);
    drawHeliRotor();
    glEndList();

    // Define the ground display list
    groundList = glGenLists(1);
    glNewList(groundList, GL_COMPILE);
    drawGround();
    glEndList();

    glClearColor(0.0, 0.0, 0.0, 0.0);
   
    // Set light position
    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
    // Create light components
    GLfloat ambientLight[] = { 0.6f, 0.6f, 0.6f, 1.0f };
    GLfloat diffuseLight[] = { 0.9f, 0.9f, 0.9f, 1.0f };
    GLfloat specularLight[] = { 0.7f, 0.7f, 0.7f, 1.0f };
    // Assign light components
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);

    timeBase = glutGet(GLUT_ELAPSED_TIME);

	textures[0] = loadTextureBMP( "Textures/ground.bmp", true, 256, 256 );

	points[0].checkpoint = 0;
	points[0].xSize = 5.0;
	points[0].ySize = 5.0;
	points[0].zSize = 0.5;
	points[0].xPos = 5.0;
	points[0].yPos = 2.5;
	points[0].zPos = 5.0;
	points[0].rotY = 0;

	points[1].checkpoint = 1;
	points[1].xSize = 5.0;
	points[1].ySize = 5.0;
	points[1].zSize = 0.5;
	points[1].xPos = -5.0;
	points[1].yPos = points[1].ySize / 2;	// Make yPos so that the bottom edge is on the ground
	points[1].zPos = 5.0;
	points[1].rotY = 60;

	points[2].checkpoint = 2;
	points[2].xSize = 5.0;
	points[2].ySize = 5.0;
	points[2].zSize = 0.5;
	points[2].xPos = -5.0;
	points[2].yPos = points[2].ySize/2;		// Make yPos so that the bottom edge is on the ground
	points[2].zPos = -5.0;
	points[2].rotY = 45;
}

GLuint loadTextureBMP( char * filename, int wrap, int width, int height )
{
	Bitmap *image;
    GLuint texture;

	image = new Bitmap();

	glEnable(GL_TEXTURE_2D);

	if (image == NULL) {
		cout << "Could not create Bitmap class.\n";
		return -1;
	}

	if (image->loadBMP(filename))
	{
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		/*// when texture area is small, bilinear filter the closest mipmap
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
		// when texture area is large, bilinear filter the first mipmap
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );*/

		// if wrap is true, the texture wraps over at the edges (repeat)
		//       ... false, the texture ends at the edges (clamp)
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap ? GL_REPEAT : GL_CLAMP );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap ? GL_REPEAT : GL_CLAMP );

		glTexImage2D(GL_TEXTURE_2D, 0, 3, image->width, image->height, 0, GL_RGB, GL_UNSIGNED_BYTE, image->data);

		glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	}
	else 
	{
		cout << "Could not load bitmap " << filename << ".\n";
		return -1;
	}

	if (image)
	{
		delete image;
	}

    return texture;
}

void drawBuilding(void)
{
	glPushMatrix();
	glColor3f(0.5, 0.5, 0.5);
	glRotatef(building0.rotY, 0.0, 1.0, 0.0);
	glTranslatef(building0.xPos, building0.yPos, building0.zPos);
	glScalef(2 * building0.xSize, 2 * building0.ySize, 2 * building0.zSize);
	glutSolidCube(1.0);
	glPopMatrix();
}

void drawHeli()
{
    glPushMatrix();
    // Go to the heli position before drawing the heli
    glTranslatef(heli.xPos, heli.yPos, heli.zPos);
    glRotatef(heli.rotY, 0.0, 1.0, 0.0);
    glRotatef(heliLeanFront, 0.0, 0.0, 1.0);
    glRotatef(heliLeanSide, 1.0, 0.0, 0.0);
	// Animate rotor
	glPushMatrix();
    glTranslatef(1.0, 0.6, 0.0);
    glRotatef(rotor, 0.0, 1.0, 0.0);
    // Draw rotor
    glCallList(heliRotorList);
	glPopMatrix();
    // Draw body
    //glCallList(heliBodyList);
	drawHeliBody();

    glPopMatrix();
}

// Draw the body
void drawHeliBody()
{
	glPushMatrix();
	//
	//     v12               v0---------------v1 \
	//     | \               |                 |  \
    //     |  v8-------------v9                |   v4
    //     |   |             |                 v5  |
    //    v13-v11------------v10               |   v6
    //                       |                 |  /
	//                       v2---------------v3 /
	//

	// Body vertices
	vertex v0 = { -0.8, 1.0, 1.0 };
	vertex v1 = { 1.4, 1.0, 1.0 };
	vertex v2 = { 0.8, -1.0, 1.0 };
	vertex v3 = { 3.0, -1.0, 1.0 };
	vertex v4 = { 2.6, 0.8, 0.5 };
	vertex v5 = { 2.2, 0.0, 1.0 };
	vertex v6 = { 3.2, 0.0, 0.5 };
	vertex v7 = { 3.0, 1.0, 1.0 };
	// Tail vertices
	vertex v8 = { -2.0, 0.4, 0.4 };
	vertex v9 = { -0.3, 0.4, 0.4 };
	vertex v10 = { -2.0, -0.4, 0.4 };
	vertex v11 = { 0.35, -0.4, 0.4 };
	// Tip of tail
	vertex v12 = {-2.3, 1.0, 0.4};
	vertex v13 = {-2.3, -0.4, 0.4};

	// Draw main body
	glColor3f(1.0, 0.0, 0.0);	// Red
	// Right Side Panel
	glBegin(GL_TRIANGLE_STRIP);
	glVertex3f(v0.x, v0.y, v0.z);
	glVertex3f(v1.x, v1.y, v1.z);
	glVertex3f(v2.x, v2.y, v2.z);
	glVertex3f(v3.x, v3.y, v3.z);
	glEnd();

	// Top Panel
	glBegin(GL_TRIANGLE_STRIP);
	glVertex3f(v0.x, v0.y, v0.z);
	glVertex3f(v1.x, v1.y, v1.z);
	glVertex3f(v0.x, v0.y, -v0.z);
	glVertex3f(v1.x, v1.y, -v1.z);
	glEnd();

	// Bottom Panel
	glBegin(GL_TRIANGLE_STRIP);
	glVertex3f(v2.x, v2.y, v2.z);
	glVertex3f(v3.x, v3.y, v3.z);
	glVertex3f(v2.x, v2.y, -v2.z);
	glVertex3f(v3.x, v3.y, -v3.z);
	glEnd();

	// Left Panel
	glBegin(GL_TRIANGLE_STRIP);
	glVertex3f(v0.x, v0.y, -v0.z);
	glVertex3f(v1.x, v1.y, -v1.z);
	glVertex3f(v2.x, v2.y, -v2.z);
	glVertex3f(v3.x, v3.y, -v3.z);
	glEnd();

	// Rear Panel
	glBegin(GL_TRIANGLE_STRIP);
	glVertex3f(v0.x, v0.y, v0.z);
	glVertex3f(v2.x, v2.y, v2.z);
	glVertex3f(v0.x, v0.y, -v0.z);
	glVertex3f(v2.x, v2.y, -v2.z);
	glEnd();

	// Tip of tail (top)
	glBegin(GL_TRIANGLE_STRIP);
	glVertex3f(v8.x, v8.y, v8.z);
	glVertex3f(v12.x, v12.y, v12.z);
	glVertex3f(v8.x, v8.y, -v8.z);
	glVertex3f(v12.x, v12.y, -v12.z);
	glEnd();

	// Tip of tail (back)
	glBegin(GL_TRIANGLE_STRIP);
	glVertex3f(v12.x, v12.y, v12.z);
	glVertex3f(v13.x, v13.y, v13.z);
	glVertex3f(v12.x, v12.y, -v12.z);
	glVertex3f(v13.x, v13.y, -v13.z);
	glEnd();

	// Tip of tail (side1)
	glBegin(GL_TRIANGLE_STRIP);
	glVertex3f(v12.x, v12.y, v12.z);
	glVertex3f(v8.x, v8.y, v8.z);
	glVertex3f(v13.x, v13.y, v13.z);
	glVertex3f(v11.x, v11.y, v11.z);
	glEnd();

	// Tip of tail (side2)
	glBegin(GL_TRIANGLE_STRIP);
	glVertex3f(v12.x, v12.y, -v12.z);
	glVertex3f(v8.x, v8.y, -v8.z);
	glVertex3f(v13.x, v13.y, -v13.z);
	glVertex3f(v11.x, v11.y, -v11.z);
	glEnd();

	// Tip of tail (bottom)
	glBegin(GL_TRIANGLE_STRIP);
	glVertex3f(v11.x, v11.y, v11.z);
	glVertex3f(v13.x, v13.y, v13.z);
	glVertex3f(v11.x, v11.y, -v11.z);
	glVertex3f(v13.x, v13.y, -v13.z);
	glEnd();

	glPushMatrix();
	glTranslatef(v2.x, v2.y, v2.z);

	
	//Chair
	glPushMatrix();
	glColor3f(1.0, 1.0, 1.0);
	glTranslatef(0.5, 0.9, -1.0);
	glScalef(0.2, 1.5, 1.5);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
	glColor3f(1.0, 1.0, 1.0);
	glTranslatef(1.0, 0.25, -1.0);
	glScalef(0.75, 0.2, 1.5);
	glutSolidCube(1.0);
	glPopMatrix();

	// Draw the Skids
	glPushMatrix();
	glColor3f(0.0, 1.0, 0.0);
	glRotatef(90, 1.0, 0.0, 0.0);
	glTranslatef(0.5, -0.5, 0.0);
	glutSolidCylinder(0.125, 0.25, 15.0, 15.0);
	glTranslatef(1.0, 0.0, 0.0);
	glutSolidCylinder(0.125, 0.25, 15.0, 15.0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(1.0, -0.3, -0.5);
	glScalef(2.5, 0.2, 0.2);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.0, 1.0, 0.0);
	glRotatef(90, 1.0, 0.0, 0.0);
	glTranslatef(0.5, -1.5, 0.0);
	glutSolidCylinder(0.125, 0.25, 15.0, 15.0);
	glTranslatef(1.0, 0.0, 0.0);
	glutSolidCylinder(0.125, 0.25, 15.0, 15.0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(1.0, -0.3, -1.5);
	glScalef(2.5, 0.2, 0.2);
	glutSolidCube(1.0);
	glPopMatrix();
	// End Skids
	glPopMatrix();

	// Front heli nose
	glBegin(GL_TRIANGLE_STRIP);
	glColor4f(1.0, 0.0, 0.0, 1.0);
	glVertex3f(v5.x, v5.y, v5.z);
	glVertex3f(v3.x, v3.y, v3.z);
	glVertex3f(v6.x, v6.y, v6.z);
	glVertex3f(v3.x, v3.y, 0.0);
	glVertex3f(v6.x, v6.y, -v6.z);
	glVertex3f(v3.x, v3.y, -v3.z);
	glVertex3f(v5.x, v5.y, -v5.z);
	glEnd();

	// Tail
	glBegin(GL_TRIANGLE_STRIP);
	// Left
	glVertex3f(v8.x, v8.y, v8.z);
	glVertex3f(v9.x, v9.y, v9.z);
	glVertex3f(v10.x, v10.y, v10.z);
	glVertex3f(v11.x, v11.y, v11.z);
	// Bottom
	glVertex3f(v10.x, v10.y, -v10.z);
	glVertex3f(v11.x, v11.y, -v11.z);
	// Right
	glVertex3f(v8.x, v8.y, -v8.z);
	glVertex3f(v9.x, v9.y, -v9.z);
	// Top
	glVertex3f(v8.x, v8.y, v8.z);
	glVertex3f(v9.x, v9.y, v9.z);
	// Back
	glVertex3f(v10.x, v10.y, -v10.z);
	glVertex3f(v10.x, v10.y, v10.z);
	glEnd();

	// Rotate cockpit windscreen
	glTranslatef(v1.x, v1.y, v1.z);		// Translate v1
	glRotatef(windscreenRot, 0.0, 0.0, 1.0);		// Rotate
	glTranslatef(-v1.x, -v1.y, -v1.z);	// Translate -v1

	// Draw cockpit window
	glColor4f(0.0, 0.0, 0.6, 0.5);	// Blue
	// Right Screen
	glBegin(GL_TRIANGLE_STRIP);
	glVertex3f(v1.x, v1.y, v1.z);
	glVertex3f(v4.x, v4.y, v4.z);
	glVertex3f(v5.x, v5.y, v5.z);
	glVertex3f(v6.x, v6.y, v6.z);
	glEnd();

	// Front top screen
	glBegin(GL_TRIANGLE_STRIP);
	glVertex3f(v1.x, v1.y, v1.z);
	glVertex3f(v4.x, v4.y, v4.z);
	glVertex3f(v1.x, v1.y, 0.0);
	glVertex3f(v4.x, v4.y, -v4.z);
	glVertex3f(v1.x, v1.y, -v1.z);
	glEnd();

	// Front mid screen
	glBegin(GL_TRIANGLE_STRIP);
	glVertex3f(v4.x, v4.y, v4.z);
	glVertex3f(v4.x, v4.y, -v4.z);
	glVertex3f(v6.x, v6.y, v6.z);
	glVertex3f(v6.x, v6.y, -v6.z);
	glEnd();

	// Left Screen
	glBegin(GL_TRIANGLE_STRIP);
	glVertex3f(v1.x, v1.y, -v1.z);
	glVertex3f(v4.x, v4.y, -v4.z);
	glVertex3f(v5.x, v5.y, -v5.z);
	glVertex3f(v6.x, v6.y, -v6.z);
	glEnd();

	glPopMatrix();
}

// Draw the rotor blades
void drawHeliRotor()
{
        glPushMatrix();
        glTranslatef(0.0, 0.8, 0.0);

        glPushMatrix();     
        // Make color blue
        glColor3f(0.0f,0.0f,1.0f);
        // Draw rotor axle
        glRotatef(90, 1.0, 0.0, 0.0);
        glutSolidCylinder(0.4, 0.4, 15, 15);
        glPopMatrix();

        glPushMatrix();
        // Make color grey
        glColor3f(0.8, 0.8, 0.8);
        // Draw blades
        glTranslatef(0.0, 0.05, 0.0);
        glScalef(4.0, 0.1, 0.2);
        glutSolidCube(1.0);
        glPopMatrix();

        glPushMatrix();
        glRotatef(90, 0.0, 1.0, 0.0);
        glScalef(4.0, 0.1, 0.2);
        glutSolidCube(1.0);
        glPopMatrix();

        glPopMatrix();
}

// Make a yellow ground square with 2 x groundSize width and length
void drawGround(void)
{
	glEnable(GL_TEXTURE_2D);
	
	glBindTexture(GL_TEXTURE_2D, textures[0]);

	// Make color yellow
	glColor4f(1.0, 1.0, 1.0, 1.0);

	// Draw the ground
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0);						glVertex3f(-groundSize, groundHeight, groundSize);
	glTexCoord2f(groundSize/2, 0.0);			glVertex3f(groundSize, groundHeight, groundSize);
	glTexCoord2f(groundSize/2, groundSize/2);	glVertex3f(groundSize, groundHeight, -groundSize);
	glTexCoord2f(0.0, groundSize/2);			glVertex3f(-groundSize, groundHeight, -groundSize);
	glEnd();

	glDisable(GL_TEXTURE_2D);
}

void drawCheckpoint(int checkpoint, float xSize, float ySize, float zSize, float rotY, float xPos, float yPos, float zPos)
{
	glPushMatrix();
	
	if (points[checkpoint].activated)
	{
		// If the checkpoint has been activated, draw green
		glColor4f(0.0, 1.0, 0.0, 0.5);
	}
	else
	{
		// Otherwise draw red
		glColor4f(1.0, 0.0, 0.0, 0.5);
	}

	glTranslatef(xPos, yPos, zPos);
	glRotatef(rotY, 0.0, 1.0, 0.0);
	glScalef(xSize, ySize, zSize);
	// draw checkpoint number?
	glutSolidCube(1.0);
	glPopMatrix();
}

void checkBounds(void)
{
    // If outside of x bounds, move to within
    if (heli.xPos < -groundSize)
    {      
        heli.xPos = -groundSize;
        eye.xPos = -groundSize + cameraDistance;
	}
    else if (heli.xPos > groundSize)
    {
        heli.xPos = groundSize;
        eye.xPos = groundSize + cameraDistance;
    }

    // If outside of z bounds, move to within
    if (heli.zPos < -groundSize)
    {
        heli.zPos = -groundSize;
        eye.zPos = -groundSize + cameraDistance;
    }
    else if (heli.zPos > groundSize)
    {
        heli.zPos = groundSize;
        eye.zPos = groundSize + cameraDistance;
    }
}

bool checkBoxCollision(objectBox object1, objectBox object2)
{
    bool collision = false;
    objectBox diff = {0, 0, 0, 0, 0, 0, 0, 0, 0};
	objectBox object1a = object1;
	objectBox object2a = object2;

	// Rotate size in x and z to correspond with rotation of the object
	object1a.xSize = cosDeg(object2a.rotY) * object2a.xSize + sinDeg(object2a.rotY) * object2a.zSize;
	object1a.zSize = -sinDeg(object2a.rotY) * object2a.xSize + cosDeg(object2a.rotY) * object2a.zSize;

	object2a.xSize = cosDeg(object2a.rotY) * object2a.xSize + sinDeg(object2a.rotY) * object2a.zSize;
	object2a.zSize = -sinDeg(object2a.rotY) * object2a.xSize + cosDeg(object2a.rotY) * object2a.zSize;

    // compute the absolute (positive) distance from object1 to object2
    diff.xPos = abs(object1.xPos - object2.xPos);
    diff.yPos = abs(object1.yPos - object2.yPos);
    diff.zPos = abs(object1.zPos - object2.zPos);
    diff.xSize = object1a.xSize + object2a.xSize;
    diff.ySize = object1a.ySize + object2a.ySize;
    diff.zSize = object1a.zSize + object2a.zSize;

    // If the distance between each of the three dimensions is within the radii combined, there is a collision
    if(diff.xPos < diff.xSize && diff.yPos < diff.ySize && diff.zPos < diff.zSize)
    {
            collision = true;
    }

    return collision;
}

bool checkPointCollision(objectBox object1, checkPoint object2)
{
	bool collision = false;

	objectBox diff = {0, 0, 0, 0};
	objectBox object1a = object1;
	checkPoint object2a = object2;

	// Rotate size in x and z to correspond with rotation of the object
	object1a.xSize = abs( cosDeg(object2a.rotY) * object2a.xSize + -sinDeg(object2a.rotY) * object2a.zSize );
	object1a.zSize = abs( sinDeg(object2a.rotY) * object2a.xSize + cosDeg(object2a.rotY) * object2a.zSize );

	object2a.xSize = cosDeg(object2a.rotY) * object2a.xSize + -sinDeg(object2a.rotY) * object2a.zSize;
	object2a.zSize = sinDeg(object2a.rotY) * object2a.xSize + cosDeg(object2a.rotY) * object2a.zSize;

	// Compute the absolute (positive) distance from object1 to object2
	diff.xPos = abs(object1.xPos - object2.xPos);
	diff.yPos = abs(object1.yPos - object2.yPos);
	diff.zPos = abs(object1.zPos - object2.zPos);
	diff.xSize = object2a.xSize;
	diff.ySize = object2a.ySize;
	diff.zSize = object2a.zSize;

    // If the distance between each of the three dimensions is within the radii combined, there is a collision
    if(diff.xPos < diff.xSize && diff.yPos < diff.ySize && diff.zPos < diff.zSize)
    {
            collision = true;
    }

	return collision;
}

/*bool checkSphereBoxCollision(object object1, objectBox object2)
{
	bool collision = false;

	objectBox diff = {0, 0, 0, 0};

    //compute the absolute (positive) distance from object1 to object2
    diff.x = abs(object1.x - object2.x);
    diff.y = abs(object1.y - object2.y);
    diff.z = abs(object1.z - object2.z);
    diff.radx = object1.rad + object2.radx;
    diff.rady = object1.rad + object2.rady;
    diff.radz = object1.rad + object2.radz;

    // If the distance between each of the three dimensions is within the radii combined, there is a collision
    if(diff.x < diff.radx && diff.y < diff.rady && diff.z < diff.radz)
    {
            collision = true;
    }

	return collision;
}*/

void checkHeliThruCollisions(void)
{
	for (int pointNum = 0; pointNum < MAX_CHECKPOINTS; pointNum++)
	{
		if ( checkPointCollision(heli, points[pointNum]) )
		{
			points[pointNum].activated = true;
		}
	}
}

void checkHeliCollisions(void)
{
    bool collision = false;

    if ( checkBoxCollision(heli, building0) )
    {
            collision = true;
    }

    if (collision)
    {
        if (movingForward)
        {
                moveHeliBack(heliSpeed, false);
        }
        else if (movingBack)
        {
                moveHeliForward(heliSpeed, false);
        }

        if (movingUp)
        {
                moveHeliDown(heliSpeed, false);
        }
        else if (movingDown)
        {
                moveHeliUp(heliSpeed, false);
        }
    }
}

// Converts degrees (in) to radians and returns the cosine (out)
float cosDeg(float degRot)
{
    float radRot = degRot * pi/180;
    return (cos(radRot));
}

// Converts degrees(in) to radians and returns the sine (out)
float sinDeg(float degRot)
{
    float radRot = degRot * pi/180;
	return (sin(radRot));
}

//start copter 
void heliStart(int i) 
{
	if (rotorSpeed < MAX_ROTOR_SPEED)
	{
		rotorSpeed = rotorSpeed + 0.2; 
		heli.yPos += 0.02; 
		eye.yPos += 0.02; 
		glutTimerFunc(40, heliStart, ++i); 
	} 
	else helicopterOn = true; 
} 

//stop copter 
void heliStop(int i) 
{ 
	if (rotorSpeed > 0)
	{ 
		rotorSpeed = rotorSpeed - 0.2; 
	} 
			if (heli.yPos > groundHeight + 1.5)
		{ 
			heli.yPos -= 0.02; 
			eye.yPos -= 0.02; 
			glutTimerFunc(40, heliStop, ++i); 
		}
			if (rotorSpeed < MAX_ROTOR_SPEED) 
				helicopterOn = false;
} 


// Catches keyboard key presses
void keyboard(unsigned char key, int mouseX, int mouseY)
{
	if(pause)
	{
		pause = false;
	}

    switch (key)
    {
        // If ESC key is pressed, exit
        case 27:
                glutLeaveMainLoop();
                break;
		case '1':
				if( windscreenRot > 0 )
					windscreenRot = 0.0;
				else
					windscreenRot = 35.0;
				glutPostRedisplay();
				break;
        case 'a':
                // Start moving the heli up
                movingUp = true;
                break;
        case 'z':
                // Start moving the heli down
                movingDown = true;
                break;
		case 's':
				// Start Blades
			heliStart(0);
			break;
		case 'x':
				//Stop Blades
			heliStop(rotorSpeed);	
			break;
    }
}

void keyboardUp(unsigned char key, int mouseX, int mouseY)
{
    switch (key)
    {
        case 'a':
                // Stop moving the heli up
                movingUp = false;
                break;
        case 'z':
                // Stop moving the heli down
                movingDown = false;
                break;
    }
}

void shadingOn()
{
	if (wire == true)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	wire = false;
}

void wireFrameOn()
{
	if (wire == false)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	wire = true;

}


// Catches special key presses
void special(int key, int mouseX, int mouseY)
{
	// If paused, unpause
	if (pause && key != GLUT_KEY_F1)
	{
		pause = false;
	}

    switch (key)
    {
        case GLUT_KEY_LEFT:
                // rotate heli left
                turningLeft = true;
                break;
        case GLUT_KEY_RIGHT:
                // rotate heli right
                turningRight = true;
                break;
        case GLUT_KEY_UP:
                // move "forward"
                movingForward = true;
                break;
        case GLUT_KEY_DOWN:
                // move "backward"
                movingBack = true;
                break;
		case GLUT_KEY_F1:
				pause = !pause;
				break;
		case GLUT_KEY_F2:
				if (wire == false)
				{
					wireFrameOn();
				}
				else
				{
					shadingOn();
				}
				break;
		case GLUT_KEY_F8:
                // turn the light/s on or off
                light0 = !light0;
				if (light0)
				{                        
					glEnable(GL_LIGHT0);
                }
                else
                {
                    glDisable(GL_LIGHT0);
                }
                break;
		case GLUT_KEY_PAGE_DOWN:
                // Zoom out
                break;
        case GLUT_KEY_PAGE_UP:
                // Zoom in
                break;
    }
}


void specialUp(int key, int mouseX, int mouseY)
{
    switch(key)
    {
        case GLUT_KEY_UP:
                // move "forward"
                movingForward = false;
                break;
        case GLUT_KEY_DOWN:
                // move "backward"
                movingBack = false;
                break;
        case GLUT_KEY_LEFT:
                // rotate heli left
                turningLeft = false;
                break;
        case GLUT_KEY_RIGHT:
                // rotate heli right
                turningRight = false;
                break;
    }
}

void mouseMotion(int x, int y)
{
    if (leftMouseDown)
    {
        float rotateY = ( (float)x - (float)last_mouse_x) / (float)windowWidth * 360.0;
		float rotateZ = ( (float)y - (float)last_mouse_y) / (float)windowHeight * 360.0;
        eye.rotY += rotateY;
        eye.rotZ += rotateZ / 2.0;
        last_mouse_x = x;
		last_mouse_y = y;

    }
}

void mouse(int button, int state, int x, int y)
{
    last_mouse_x = x;
	last_mouse_y = y;

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        leftMouseDown = true;
    }
    else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN)
    {
        eye.rotY = 135;
		eye.rotX = 0;
		eye.rotZ = 0;
    }
    else
    {
        leftMouseDown = false;
    }
}

void moveHeliForward(float speed, bool checkCol)
{
    // Move heli
    heli.xPos += speed * cosDeg(heli.rotY);
    heli.zPos -= speed * sinDeg(heli.rotY);

    // Move camera
    eye.xPos += speed * cosDeg(heli.rotY);
    eye.zPos -= speed * sinDeg(heli.rotY);

    checkHeliCollisions();
	checkHeliThruCollisions();
}

void moveHeliBack(float speed, bool checkCol)
{
    // Move heli
    heli.xPos -= speed * cosDeg(heli.rotY);
    heli.zPos += speed * sinDeg(heli.rotY);

    // Move camera
    eye.xPos -= speed * cosDeg(heli.rotY);
    eye.zPos += speed * sinDeg(heli.rotY);

    checkHeliCollisions();
	checkHeliThruCollisions();
}

void moveHeliUp(float speed, bool checkCol)
{
    heli.yPos += heliSpeed;
    eye.yPos += heliSpeed;

    checkHeliCollisions();
	checkHeliThruCollisions();
}

void moveHeliDown(float speed, bool checkCol)
{
    heli.yPos -= heliSpeed;
    eye.yPos -= heliSpeed;

    checkHeliCollisions();
	checkHeliThruCollisions();
}

void updateGameTime()
{
	if (!pause)
	{
		gameTime += time - gameTimeBase;	// Increment the time spent playing
		int gameTimeMillisec = (gameTime % 1000) / 10;		// Get milliseconds from game time
		int gameTimeSeconds = (gameTime % 60000) / 1000;	// Get seconds from game time
		int gameTimeMinutes = (gameTime % 3600000) / 60000;	// Get minutes from game time
		sprintf(strGameTime, "Time: %.2i:%.2i:%.2i", gameTimeMinutes, gameTimeSeconds, gameTimeMillisec);	
	}

	gameTimeBase = time;
}

void updateFPS()
{
    // Update the FPS every second
    frames++;
    time = glutGet(GLUT_ELAPSED_TIME);

    if (time - timeBase > 1000) // If a second has passed
    {
        fps = frames * 1000.0 / (time - timeBase);              // calculate FPS
        sprintf(strFps, "FPS: %4.2f", fps);		// get the string value of integer FPS
        timeBase = time;        // Set the base time to current time
        frames = 0;     // Reset the frame count
    }
}

void displayText()
{
    // Write text to screen
    glPushMatrix();
    glColor3f(0.0, 1.0, 1.0);
    setOrthographicProjection();
    glLoadIdentity();
	// Display FPS
    renderBitmapString(textX, textY, (void *)font, strFps);
	// Display game time
	renderBitmapString(textX, textY + 25, (void *)font, strGameTime);

	// If paused, display help/controls/scores
	if (pause)
	{
		displayHelp();
	}

    resetPerspectiveProjection();
    glPopMatrix();
}

void displayHelp()
{
	const int HELP_SPACE = 20;
	const int HELP_YPOS = 100;
	const float MARGIN = 5.0;

	glColor4f(1.0, 0.7, 0.2, 1.0);

	renderBitmapString(HELP_SPACE, HELP_YPOS, (void *)font, "F1 - Pause and bring up this screen");
	renderBitmapString(HELP_SPACE, HELP_YPOS + HELP_SPACE, (void *)font, "F2 - Switch between wireframe and solid shapes");
	renderBitmapString(HELP_SPACE, HELP_YPOS + 2*HELP_SPACE, (void *)font, "F3 - Switch between textures and no textures");
	renderBitmapString(HELP_SPACE, HELP_YPOS + 3*HELP_SPACE, (void *)font, "F8 - Switch Light 0 on/off");
	renderBitmapString(HELP_SPACE, HELP_YPOS + 4*HELP_SPACE, (void *)font, "a/z - Move helicopter up/down");
	renderBitmapString(HELP_SPACE, HELP_YPOS + 5*HELP_SPACE, (void *)font, "Directional Arrows - Move forward/backward");
	renderBitmapString(2*HELP_SPACE, HELP_YPOS + 6*HELP_SPACE, (void *)font, " and Turn left/right");
	renderBitmapString(HELP_SPACE, HELP_YPOS + 7*HELP_SPACE, (void *)font, "s/x - Start/stop engine");
	renderBitmapString(HELP_SPACE, HELP_YPOS + 8*HELP_SPACE, (void *)font, "Right Mouse - Brings up game menu");
	renderBitmapString(HELP_SPACE, HELP_YPOS + 9*HELP_SPACE, (void *)font, "Drag Left Mouse - Rotate camera around helicopter");
	renderBitmapString(HELP_SPACE, HELP_YPOS + 10*HELP_SPACE, (void *)font, "Middle Mouse - Reset camera to chase position");

	// Display the best time
	int bestTimeMinutes = (bestTime % 1000) / 10;
	int bestTimeSeconds = (bestTime % 60000) / 1000;
	int bestTimeMillisec = (bestTime % 3600000) / 60000;

	sprintf(strBestTime, "Best Time: %.2i:%.2i:%.2i", bestTimeMinutes, bestTimeSeconds, bestTimeMillisec);
	renderBitmapString(HELP_SPACE, 350, (void *)font, strBestTime);

	// Display who wrote the project and it's purpose
	renderBitmapString(HELP_SPACE, 400, (void *)font, "Written by Aleesha, Ashley and Chris");
	renderBitmapString(HELP_SPACE, 425, (void *)font, " for Cp2060 Assignment 2");

	// Display a backing screen so that the text is readable
	glColor4f(0.5, 0.5, 0.5, 0.8);	// Grey
	glBegin(GL_QUADS);
	glVertex2f(MARGIN, MARGIN);
	glVertex2f(windowWidth - MARGIN, MARGIN);
	glVertex2f(windowWidth - MARGIN, windowHeight - MARGIN);
	glVertex2f(MARGIN, windowHeight - MARGIN);
	glEnd();
}

// These next three functions are taken from Lighthouse 3D tutorials:
// http://www.lighthouse3d.com/opengl/glut/index.php?bmpfontortho
void setOrthographicProjection()
{
    // switch to projection mode
    glMatrixMode(GL_PROJECTION);
    // save previous matrix which contains the
    //settings for the perspective projection
    glPushMatrix();
    // reset matrix
    glLoadIdentity();
    // set a 2D orthographic projection
    gluOrtho2D(0, windowWidth, 0, windowHeight);
    // invert the y axis, down is positive
    glScalef(1, -1, 1);
    // move the origin from the bottom left corner
    // to the upper left corner
    glTranslatef(0, -windowHeight, 0);
    glMatrixMode(GL_MODELVIEW);
}

// Set projection back to it's starting point
void resetPerspectiveProjection()
{
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

// Draw the string on the screen
void renderBitmapString(float x, float y, void *font,char *string)
{
    char *c;
    // Set the draw co-ordinates
    glRasterPos2f(x, y);
    for (c=string; *c != '\0'; c++)
    {
        // Display each character in the array
        glutBitmapCharacter(font, *c);
    }
}
// End lighthouse 3D code

// When there's nothing else to do, update animation
void idle(void)
{
	if(!pause)
	{
		if (helicopterOn == true)
		{
			if (movingForward)
			{
				// move forward
				moveHeliForward(heliSpeed, true);
				heliLeanFront = -LEAN_FACTOR;
			
			}
			else if (movingBack)
			{
				// move back
				moveHeliBack(heliSpeed, true);
				heliLeanFront = LEAN_FACTOR;
			}
			else
			{
				// Reset the front lean and straighten up
				heliLeanFront = 0;
			}

			if (turningLeft)
			{
				// turn left
				heli.rotY += rotSpeed;
				// Adjust the rotor spin to counter heli spin
				rotor += rotorSpeed - rotSpeed;
				heliLeanSide = -LEAN_FACTOR;
			}
			else if (turningRight)
			{
				// turn right
				heli.rotY -= rotSpeed;
				// Adjust the rotor spin to counter heli spin
				rotor += rotorSpeed + rotSpeed;
				heliLeanSide = LEAN_FACTOR;
			}
		
			else
			{
				// Turn the rotor normally
				rotor += rotorSpeed;
				// Reset the lean and straighten up
				heliLeanSide = 0;
			}
		}
		else
		{
			//turn the rotor normally
			rotor += rotorSpeed;
			heliLeanSide = 0;
		}

		if (helicopterOn == true)
		{
			if (movingUp)
			{
				moveHeliUp(heliSpeed, true);
			}
			else if (movingDown)
			{
				if(heli.yPos > groundHeight + heli.ySize/2.0)
				{
					moveHeliDown(heliSpeed, true);
				}
			}
		}

		// Make sure heli is in the level's bounds
        checkBounds();
	}

	glutPostRedisplay();
}



// When the window is reshaped, this function updates the camera and display
void reshape(int w, int h)
{
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-1.0, 1.0, -1.0, 1.0, 1.5, 20.0);
    glMatrixMode(GL_MODELVIEW);
}


// Display the scene and it's components
void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(eye.xPos, eye.yPos, eye.zPos, heli.xPos, heli.yPos, heli.zPos, 0.0, 1.0, 0.0);

    // Rotate camera so that it is always behind the heli
    glPushMatrix();
    glTranslatef(heli.xPos, heli.yPos, heli.zPos);
	glRotatef( eye.rotZ, 0.0, 0.0, 1.0);
	glRotatef(-heli.rotY + eye.rotY, 0.0, 1.0, 0.0);
    glTranslatef(-heli.xPos, -heli.yPos, -heli.zPos);

    // Draw ground
    glPushMatrix();
    glTranslatef(0, groundHeight, 0);
	drawGround();
    glPopMatrix();

    // Draw building
    glPushMatrix();
    drawBuilding();
    glPopMatrix();

	// Draw the helicopter
	drawHeli();

	// Draw the checkpoints
	for (int checkPoint = 0; checkPoint < MAX_CHECKPOINTS; checkPoint++)
	{
		drawCheckpoint( points[checkPoint].checkpoint, points[checkPoint].xSize, points[checkPoint].ySize, points[checkPoint].zSize, points[checkPoint].rotY, points[checkPoint].xPos, points[checkPoint].yPos, points[checkPoint].zPos);
	}

	updateFPS();
	updateGameTime();
	displayText();

    glPopMatrix();

    glutSwapBuffers();
}

//adding menu

void mymenu(int choice)
{

	switch (choice) {
	case 1: shadingOn(); // Full Shading
		break;
	case 2: wireFrameOn();  // Wireframe
		break;
	case 3: pause = !pause; // Help/Credits/Scores
		break;
	case 4: exit(0);  // exit program
		break;
	default: break;
	}
}


int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(windowPosWidth, windowPosHeight);
    glutCreateWindow("Cp2060 Assignment 2 - Heliaac");
    init();
    glutDisplayFunc(display);
    glutSpecialFunc(special);
    glutSpecialUpFunc(specialUp);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutMouseFunc(mouse);
    glutMotionFunc(mouseMotion);

	// create main menu (no ID needed)
	glutCreateMenu(mymenu);
	glutAddMenuEntry("Full Shading", 1);
	glutAddMenuEntry("Wire Frame", 2);
	glutAddMenuEntry("Help/Credits/Scores", 3);
	glutAddMenuEntry("Exit Program", 4);
	// set the menu to the right button
	glutAttachMenu(GLUT_RIGHT_BUTTON);  

    glutMainLoop();
    return 0;
}