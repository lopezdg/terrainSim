#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/* SE 3GC3: Computer Graphics - Assignment 2	
 * Gabriel Lopez de Leon, lopezdg, 1310514
 * 11 November 2015
 */

/* display function - GLUT display callback function
 *		clears the screen, draws a square, and displays it
 */

 //following variables used for the camera
float camPos[] = {6, 6, 12}; //initial camera angle when the program is run
float angle = 0.0f;
float pos[] = {2,2,2};
float rot[] = {0, 0, 0};
float light_pos[] = {2,2,2, 1};

//terrain related globals
int tSizeX = 0; //x dimension of the terrain
int tSizeZ = 0; //z dimension of the terrain
float terrain[300][300]; //this is the maximum size of the terrain
bool toCircle[300][300];
float tCircleSize = 30; //size of the circles used to generate the terrain

//variable to check if user input is within range
bool checkInput = false;

bool lightOn = false;
int wireFrame = 0; //0 = wireframe toggle off, 1 = wireframe toggle on, 2 = wireframe + solid polygons

//this algorithm is from assignment 2 pdf found on avenue and is used to produce height values
//this algorithm picks a random point which will be the centre of a circle which is then displaced up or down, along with the points around it (where the centre point is the highest or lowest point in the circle)
void circleAlg(int centerX, int centerZ, float disp){
	
	for (int i = 0; i < tSizeX-1; i++){ //loops through all of the components of the array terrain[][]
		for (int j = 0; j < tSizeZ-1; j++){
			//the following equation is used to find the distance from the centre of the circle and follows the pseudocode provided with the algorithm
			float distance = sqrt(pow((i-centerX),2) + pow((j-centerZ),2));

			float pd = (distance * 2) / tCircleSize;

			if (fabs(pd) <= 1.0 && pd != 0){
				terrain[i][j] += (disp/2 + cos(pd*3.14) * disp/2);
			}
		}
	}

	//For some reason the center of the circle had a different height.
	//This ensures that is is the appropriate height
	terrain[centerX][centerZ] = (terrain[centerX+1][centerZ] + terrain[centerX][centerZ+1]) / 2;
}

void resetTerrain(){ //the following is used to reset the terrain which is generated, it sets all of the values in the 2D array to zero where the terrain can once again be re-generated later on
		for (int i = 0; i < tSizeX; i++){
			for (int j = 0; j < tSizeZ; j++){
				toCircle[i][j] = false;
				terrain[i][j] = 0;
			}
		}
}

void createTerrain(){
	//following is used to create the terrain by filling the 2D array terrain[][] with random values which will be later scaled down to be between 0 and 1
	for (int i = 1; i < tSizeX-1; i++){
		for (int j = 1; j < tSizeZ-1; j++){

			if (rand() % 1000 >= 995){
				terrain[i][j] = (rand() % 100)/100; //uses the rand() function to choose random value which will populate the array.
				toCircle[i][j] = true;
			}
			else
				terrain[i][j] = 0;
		}
	}

	//Run points through the algorithm provided on the assignment page
	for (int i = 0; i < tSizeX; i++){
		for (int j = 0; j < tSizeZ; j++){
			if(toCircle[i][j])
				circleAlg(i,j,1); //runs the circle algorithm given the populated array
		}
	}
}

//function to render the terrain
void drawTerrain(float* pos, float* rot)
{
	for (int i = 0; i < tSizeX-1; i++){
		for (int j = 0; j < tSizeZ-1; j++){
			glBegin(GL_QUAD_STRIP);
			glColor3f(0.6 + 0.1*terrain[i][j], 0.6 + 0.2*terrain[i][j], 0.6 + 0.3*terrain[i][j]);

			// Each x,z quad is scaled by 0.1 since the values of the terrain were much too large when set earlier
			float scaledX = i*0.1;
			float scaledZ = j*0.1;

			glVertex3f(scaledX, terrain[i][j], scaledZ);
			glVertex3f(scaledX + 0.1, terrain[i+1][j], scaledZ);
			glVertex3f(scaledX, terrain[i][j+1], scaledZ + 0.1);
			glVertex3f(scaledX + 0.1, terrain[i+1][j+1], scaledZ + 0.1);
			glEnd();
		}
	}
	
	glFlush();
}

void keyboard(unsigned char key, int x, int y){
	switch(key){

		case 'q': //use of lower case letters for the cases so as to not require the user to press multiple keys
			exit(0); //used to close the program
			break;
		
		case 'r':
			resetTerrain(); //calls the reset function
			createTerrain(); //generates the terrain again after it was resetted 
			break;
			
		case 'a':
			if(light_pos[0] > -15){
				light_pos[0]-=1;
				// printf("\nleft");
			}
			break;

		case 'w':
			if(light_pos[2] > -15){
				light_pos[2] -= 1;
				// printf("\nup");
			}
			break;

		case 'd':
			if(light_pos[0] < 15){
				light_pos[0]+=1;
				// printf("\nright");
			}
			break;

		case 's':
			if(light_pos[2] < 15){
				light_pos[2] += 1;
				// printf("down\n");
			}
			break;

		case 'z':
			if(light_pos[1] < 15){
				light_pos[1] += 1;
				// printf("\nrUp");
			}
			break;

		case 'x':
			if(light_pos[1] > 0){
				light_pos[1] -= 1;
				// printf("\nrDown");
			}
			break;
			
		case 'l':
			if(lightOn == true){
				glDisable(GL_LIGHTING);
				lightOn = false;
			}
			else{
				glEnable(GL_LIGHTING);
				lightOn = true;
			}
			break;
			
		case 'e':
			if(wireFrame == 0){
				glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );//sets the wireframe on 
				wireFrame = 1;
			}
			else if (wireFrame == 1){ //this is supposed to turn on wireframe with the solid polygons
				wireFrame = 2;
			}
			else if (wireFrame == 2){
				glPolygonMode( GL_FRONT_AND_BACK, GL_FILL ); //this disables wireframe
				wireFrame = 0;
			}
			break;
			
	}
	glutPostRedisplay(); //this ensures that the display function is called once again and will redraw the terrain when it is reset
}

//code based on the sample snowman program on avenue
//the following sets the arrow keys to adjust the camera angle and sets the 'home' and 'end' key to rotate up and down respectively
void special(int key, int x, int y)
{
	/* arrow key presses move the camera */
	switch(key)
	{
		case GLUT_KEY_LEFT:
			camPos[0]-=0.2;
			break;

		case GLUT_KEY_RIGHT:
			camPos[0]+=0.2;
			break;

		case GLUT_KEY_UP:
			camPos[2] -= 0.2;
			break;

		case GLUT_KEY_DOWN:
			camPos[2] += 0.2;
			break;
		
		case GLUT_KEY_HOME:
			camPos[1] += 0.2;
			break;

		case GLUT_KEY_END:
			camPos[1] -= 0.2;
			break;

	}
	glutPostRedisplay(); //ensures the screen refreshes 
}

void init(void)
{
	glClearColor(0, 0, 0, 0);
	glColor3f(1, 1, 1);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, 1, 1, 100);
	
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT1);
		lightOn = true;

	//float light_pos[4] = {2,2,2, 1};

	float amb[4] = {0.4, 0.4, 0.4, 1};
	float diff[4] = {0.8,0.8,0.8, 1};
	float spec[4] = {0.2,0.2,.2, 1};


	glLightfv(GL_LIGHT1, GL_POSITION, light_pos);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diff);
	glLightfv(GL_LIGHT1, GL_AMBIENT, amb);
	glLightfv(GL_LIGHT1, GL_SPECULAR, spec);
	
	//Face Culling from tutorial 5 
	glFrontFace(GL_CCW);
	glCullFace(GL_FRONT);
	glEnable(GL_CULL_FACE);
}

void display(void)
{
	float m_amb[] = {0.2, 0.2, 0.2, 1.0};
	float m_dif[] = {0.8, 0.8, 0.8, 1.0};
	float m_spec[] = {0.2, 0.2, 0.2, 1.0};
	float shiny = 27;
	
	float origin[3] = {0,0,0};
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(camPos[0], camPos[1], camPos[2], 0,0,0, 0,1,0);
	glColor3f(1,1,1);
	glEnable(GL_COLOR_MATERIAL);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m_amb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m_dif);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, m_spec);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);

	drawTerrain(pos, rot);
	
	glutSwapBuffers();
} 

//callbacks
void glutCallbacks(){
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
}

//list of cases for the different menu options
void menu(int value){
	printf("Menu Interaction\n");
	switch(value){
		case 0: 
			printf("Terrain size is %d x %d\n", tSizeX, tSizeZ);
			break;
		case 1:
			resetTerrain();
			createTerrain();
			break;
		case 2:
			exit(0);
			break;
	}
	glutPostRedisplay();
}

static int id;
//creates a menu upon using the right mouse button
void initMenu(){
	id = glutCreateMenu(menu);
	glutAddMenuEntry("Show Terrain Size", 0);
	glutAddMenuEntry("Reset", 1);
	glutAddMenuEntry("Quit", 2);

	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

//this function asks the user to input the array size when the program is run. It will only accept values between 50 and 300
void askInput(){
	printf("Input Terrain size between 50 and 300: ");
    scanf("%d",&tSizeX);
	tSizeZ = tSizeX;
	if (tSizeX > 300 || tSizeX < 50){
		checkInput = false;
		printf("That input is invalid, please try again.\n");
	}
	else{
		checkInput = true;
	}
}

//The following is used as the main constructor of the code
int main(int argc, char** argv)
{
	while(checkInput == false){ //loops through the function to check if the input is valid, if not it will keep looping, else it will exit the loop and draw the terrain
		askInput();
	}
	createTerrain();
	
	glutInit(&argc, argv);	//This is used to start up GLUT
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(500, 100);
	glutCreateWindow("Terrain: lopezdg - 1310514 ");	//creates the window and sets the title
	glutCallbacks();
	initMenu();
	glEnable(GL_DEPTH_TEST);
	init();
	printf("\n\n\n\n---------------------------------\nTerrain by Gabriel Lopez de Leon\n---------------------------------\nList of Controls:\nAdjust Light Position = 'a,w,s,d'\nAdjust Camera = Arrow Keys\nRotate Up = 'Home'\nRotate Down = 'End'\nToggle Lighting = 'l'\nToggle Wireframe = 'e'\nReset = 'r'\nExit = 'q'");
	glutMainLoop();	//starts the event loop which occurs when the program is running
	return(0);
}