// Assignment 2.cpp by Christopher Trott
// Created on 28/08/09 
// Last Modified on 28/08/09 @ 15:03
//
// Some of this code is taken from animlightpos.cpp on the LearnJCU resources page
// Some code to do with lighting was gained from the URL: http://www.falloutsoftware.com/tutorials/gl/gl8.htm
// Some code to do with text on screen gained from Lighthouse 3D @ URL: http://www.lighthouse3d.com/opengl/glut/index.php?bmpfontortho

#include<GL/freeglut.h>
#include<math.h>
#include<string>

using namespace std;

struct vector
{
	float x;
	float y;
	float z;
};

struct object
{
	float x;	// x position
	float y;	// y position
	float z;	// z position
	float rot;	// rotation angle of direction
	float rad;	// radius of bounding sphere
};

struct objectBox
{
	float x;	// x position
	float y;	// y position
	float z;	// z position
	float rot;	// rotation angle of direction
	float radx;	// x radius (half-length) of bounding box
	float rady;	// y radius of bounding box
	float radz;	// z radius of bounding box
};

void drawHeliBody(void);
void drawHeliRotor(void);
void drawGround(void);
void moveHeliForward(float speed, bool checkCol);
void moveHeliBack(float speed, bool checkCol);
void moveHeliDown(float speed, bool checkCol);
void moveHeliUp(float speed, bool checkCol);
void checkBounds(void);
bool checkSphereCollision(object object1, object object2);
void drawBuilding(void);

float cameraDistance = 5.0;
objectBox heli = {0, 0, 0, 0, 2.4, 1, 2.4};

objectBox eye = {cameraDistance, heli.y, cameraDistance, 135, 0, 0, 0};
objectBox building0 = {10, 0, 10, 0, 2, 5, 2};

bool movingForward = false;
bool movingBack = false;
bool movingUp = false;
bool movingDown = false;
bool turningLeft = false;
bool turningRight = false;

int font = (int)GLUT_BITMAP_HELVETICA_18;
int textX = 20;
int textY = 20;

bool lighting = false;
bool lights = false;

float x = 1, y = 20, z = 1;
GLfloat light_position[] = { x, y, z, 0 };

GLuint heliBodyList;
GLuint heliRotorList;
GLuint groundList;

int rotor = 0;
int rotorSpeed = 10;
int groundSize = 20;

int windowWidth = 500;
int windowHeight = 500;

int windowPosWidth = 100;
int windowPosHeight = 100;

int frames = 0;
int time = 0;
int timeBase = 0;
float fps = 50.0;
char* strFps = new char[4];	// FPS string for display on-screen

const int ROTATE_SPEED = 180;
const int HELI_SPEED = 12;
const float LEAN_FACTOR = 15.0;
float heliLeanFront = 0.0;
float heliLeanSide = 0.0;
int rotSpeed = ROTATE_SPEED / fps;
float heliSpeed = HELI_SPEED / fps;

int last_mouse_x = 0;
bool rightMouseDown = false;

float pi = 3.1415926535897932384626433832795;

// Initialise the OpenGL properties
void init(void)
{
	glEnable(GL_DEPTH_TEST);
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
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	// Create light components
	GLfloat ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat diffuseLight[] = { 0.8f, 0.8f, 0.8, 1.0f };
	GLfloat specularLight[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	// Assign light components
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);

	timeBase = glutGet(GLUT_ELAPSED_TIME);
}

void drawBuilding(void)
{
	glPushMatrix();
	glColor3f(0.5, 0.5, 0.5);
	glRotatef(building0.rot, 0.0, 1.0, 0.0);
	glTranslatef(building0.x, building0.y, building0.z);
	glScalef(2 * building0.radx, 2 * building0.rady, 2 * building0.radz);
	glutSolidCube(1.0);
	glPopMatrix();
}

// Draw the body
void drawHeliBody()
{
	glPushMatrix();
	// Make color red
	glTranslatef(0.6, 0.0, 0.0);
	glColor3f(1.0, 0.0, 0.0);
	glutSolidCube(2.0);
	// Draw the tail boom
	glTranslatef(-1.2, 0.0, 0.0);
	glScalef(1.8, 0.4, 0.4);
	glutSolidCube(2.0);
	glPopMatrix();
}

// Draw the rotor blades
void drawHeliRotor()
{
	glPushMatrix();
	glTranslatef(0.0, 1.2, 0.0);

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
	glTranslatef(0.0, 0.1, 0.0);
	glScalef(4.0, 0.2, 0.2);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
	glRotatef(90, 0.0, 1.0, 0.0);
	glScalef(4.0, 0.2, 0.2);
	glutSolidCube(1.0);
	glPopMatrix();

	glPopMatrix();
}

// Make a yellow ground square with 2 x groundSize width and length
void drawGround(void)
{
	// Make color yellow
	glColor3f(1.0, 1.0, 0.0);
	// Draw the ground
	glBegin(GL_QUADS);
	glVertex3f(-groundSize, 0, -groundSize);
	glVertex3f(groundSize, 0, -groundSize);
	glVertex3f(groundSize, 0, groundSize);
	glVertex3f(-groundSize, 0, groundSize);
	glEnd();
}

void checkBounds(void)
{
	// If outside of x bounds, move to within
	if (heli.x < -groundSize)
	{	
		heli.x = -groundSize;
		eye.x = -groundSize + cameraDistance;
	}
	else if (heli.x > groundSize)
	{
		heli.x = groundSize;
		eye.x = groundSize + cameraDistance;
	}

	// If outside of z bounds, move to within
	if (heli.z < -groundSize)
	{
		heli.z = -groundSize;
		eye.z = -groundSize + cameraDistance;
	}
	else if (heli.z > groundSize)
	{
		heli.z = groundSize;
		eye.z = groundSize + cameraDistance;
	}
}

bool checkSphereCollision(object object1, object object2)
{
	bool collision = false;
	object diff = {0, 0, 0, 0};

	// compute the absolute (positive) distance from object1 to object2
	diff.x = abs(object1.x - object2.x);
	diff.y = abs(object1.y - object2.y);
	diff.z = abs(object1.z - object2.z);
	diff.rad = object1.rad + object2.rad;

	// If the distance between each of the three dimensions is within the radii combined, there is a collision
	if(diff.x < diff.rad && diff.y < diff.rad && diff.z < diff.rad)
	{
		collision = true;
	}

	return collision;
}

bool checkBoxCollision(objectBox object1, objectBox object2)
{
	bool collision = false;
	objectBox diff = {0, 0, 0, 0};

	// compute the absolute (positive) distance from object1 to object2
	diff.x = abs(object1.x - object2.x);
	diff.y = abs(object1.y - object2.y);
	diff.z = abs(object1.z - object2.z);
	diff.radx = object1.radx + object2.radx;
	diff.rady = object1.rady + object2.rady;
	diff.radz = object1.radz + object2.radz;

	// If the distance between each of the three dimensions is within the radii combined, there is a collision
	if(diff.x < diff.radx && diff.y < diff.rady && diff.z < diff.radz)
	{
		collision = true;
	}

	return collision;
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
float cosDeg(float heliRot)
{
	float radRot = heliRot * pi/180;
	return (cos(radRot));
}

// Converts degrees(in) to radians and returns the sine (out)
float sinDeg(float heliRot)
{
	float radRot = heliRot * pi/180;
	return (sin(radRot));
}

// Catches keyboard key presses
void keyboard(unsigned char key, int mouseX, int mouseY)
{
	switch (key)
	{
		// If ESC key is pressed, exit
		case 27:
			glutLeaveMainLoop();
			break;
		case 'a':
			// Start moving the heli up
			movingUp = true;
			break;
		case 'z':
			// Start moving the heli down
			movingDown = true;
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

// Catches special key presses
void special(int key, int mouseX, int mouseY)
{
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
		case GLUT_KEY_F9:
			// turn the light/s on or off
			lights = !lights;
			if (lights)
			{
				glEnable(GL_LIGHT0);
			}
			else
			{
				glDisable(GL_LIGHT0);
			}
			break;
		case GLUT_KEY_F10:
			// Enable/Disable lighting
			lighting = !lighting;
			if (lighting)
			{
				glEnable(GL_LIGHTING);
			}
			else
			{
				glDisable(GL_LIGHTING);
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
	if (rightMouseDown)
	{
		float rotate = ( (float)x - (float)last_mouse_x) / (float)windowWidth * 360.0;
		eye.rot += rotate;
		last_mouse_x = x;
	}
}

void mouse(int button, int state, int x, int y)
{
	last_mouse_x = x;

	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		rightMouseDown = true;
	}
	else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN)
	{
		eye.rot = 135;
	}
	else
	{
		rightMouseDown = false;
	}
}

void moveHeliForward(float speed, bool checkCol)
{
	// Move heli
	heli.x += speed * cosDeg(heli.rot);
	heli.z -= speed * sinDeg(heli.rot);

	// Move camera
	eye.x += speed * cosDeg(heli.rot);
	eye.z -= speed * sinDeg(heli.rot);

	checkHeliCollisions();
}

void moveHeliBack(float speed, bool checkCol)
{
	// Move heli
	heli.x -= speed * cosDeg(heli.rot);
	heli.z += speed * sinDeg(heli.rot);

	// Move camera
	eye.x -= speed * cosDeg(heli.rot);
	eye.z += speed * sinDeg(heli.rot);

	checkHeliCollisions();
	
}

void moveHeliUp(float speed, bool checkCol)
{
	heli.y += heliSpeed;
	eye.y += heliSpeed;

	checkHeliCollisions();
}

void moveHeliDown(float speed, bool checkCol)
{
	heli.y -= heliSpeed;
	eye.y -= heliSpeed;

	checkHeliCollisions();
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
		heliLeanFront = 0;
	}

	if (turningLeft)
	{
		// turn left
		heli.rot += rotSpeed;
		// Adjust the rotor spin to counter heli spin
		rotor += rotorSpeed - rotSpeed;
		heliLeanSide = -LEAN_FACTOR;
	}
	else if (turningRight)
	{
		// turn right
		heli.rot -= rotSpeed;
		// Adjust the rotor spin to counter heli spin
		rotor += rotorSpeed + rotSpeed;
		heliLeanSide = LEAN_FACTOR;
	}
	else
	{
		// Turn the rotor normally
		rotor += rotorSpeed;
		heliLeanSide = 0;
	}

	if (movingUp)
	{
		moveHeliUp(heliSpeed, true);
	}
	else if (movingDown)
	{
		moveHeliDown(heliSpeed, true);
	}

	checkBounds();

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

	gluLookAt(eye.x, eye.y, eye.z, heli.x, heli.y, heli.z, 0.0, 1.0, 0.0);

	// Rotate camera so that it is always behind the heli
	glPushMatrix();
	glTranslatef(heli.x, heli.y, heli.z);
	glRotatef(-heli.rot + eye.rot, 0.0, 1.0, 0.0);
	glTranslatef(-heli.x, -heli.y, -heli.z);

	// Draw the helicopter
	glPushMatrix();
	// Go to the heli position before drawing the heli
	glTranslatef(heli.x, heli.y, heli.z);
	glRotatef(heli.rot, 0.0, 1.0, 0.0);
	glRotatef(heliLeanFront, 0.0, 0.0, 1.0);
	glRotatef(heliLeanSide, 1.0, 0.0, 0.0);
	// Draw body
	glCallList(heliBodyList);
	// Animate rotor
	glTranslatef(0.6, 0.0, 0.0);
	glRotatef(rotor, 0.0, 1.0, 0.0);
	// Draw rotor
	glCallList(heliRotorList);
	glPopMatrix();

	// Draw ground
	glPushMatrix();
	glTranslatef(0, -5, 0);
	glCallList(groundList);
	glPopMatrix();

	// Draw building
	glPushMatrix();
	drawBuilding();
	glPopMatrix();

	// Update the FPS every second
	frames++;
	time = glutGet(GLUT_ELAPSED_TIME);
	
	if (time - timeBase > 1000) // If a second has passed
	{
		fps = frames * 1000.0 / (time - timeBase);		// calculate FPS
		sprintf(strFps, "FPS: %4.2f", fps);
	 	timeBase = time;	// Set the base time to current time
		frames = 0;	// Reset the frame count
	}

	// Write text to screen
	glPushMatrix();
	glColor3f(0.0, 1.0, 1.0);
	setOrthographicProjection();
	glLoadIdentity();
	renderBitmapString(textX, textY, (void *)font, strFps);
	resetPerspectiveProjection();
	glPopMatrix();

	glPopMatrix();

	glutSwapBuffers();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition(windowPosWidth, windowPosHeight);
	glutCreateWindow("Cp2060 Assignment 1 - Christopher Trott");
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
	glutMainLoop();
	return 0;
}