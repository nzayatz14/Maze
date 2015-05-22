//============================================================================
// Name        : maze.cpp
// Author      : Nick Zayatz
// Version     :
// Copyright   : Your copyright notice
// Description : Maze game for Graphics 1.
//
// Sample Inputs From Command Line:
// maze
// maze <widthOfMaze> <heightOfMaze>
// maze <widthOfMaze> <heightOfMaze> <numberOfBacktracesAllowed>
//
// WARNING: TEXT OUTPUT DOES NOT PRINT ON MACS
//============================================================================

#include <iostream>
#include <climits>
#include <cstdlib>
#include <time.h>
#include <vector>
#include "border.h"
#include "gridSpace.h"

using namespace std;

//variables pertaining to the matrix
int rows, cols;
int startBackTracks, backTracks;
int numBorders;
int numSpaces;
int verticals;
float spaceBetweenBorders;

//variables pertaining to the users position
int currentSpace;
int prevSpace;
int startSpace, endSpace;

//variables pertaining to the walls and crumbs
border *borders;
gridSpace *crumbs;
vector<int> vectorCrumbs;

//variables pertaining to winning the game
bool won;
char winMessage[] = "You Win!";
char loseMessage[] = "You lose :(";

//create variables to hold the beginning world coordinates and coordinates that can be adjusted while resizing the window
const GLfloat wld_left = -0.1;
const GLfloat wld_right = 1.1;
const GLfloat wld_bot = -0.1;
const GLfloat wld_top = 1.1;
const GLfloat wld_near = -1.0;
const GLfloat wld_far = 1.0;

GLfloat curr_wld_left = wld_left;
GLfloat curr_wld_right = wld_right;
GLfloat curr_wld_bot = wld_bot;
GLfloat curr_wld_top = wld_top;
GLfloat curr_wld_near = wld_near;
GLfloat curr_wld_far = wld_far;

//create mat4's and locations
mat4 p, mv;

GLint projLoc;
GLint mvLoc;

//hold the current width and height of the screen
GLint width, height;

void init();
void display( void );
void mouse(int x, int y);
void keyboard( unsigned char key, int x, int y );
void myMenu(int item);
void set_up_menu( void (*myMenu)(int));
bool inOneSet(int array[], int size);
int findLeader(int array[], int start);
void myReshape(GLint w, GLint h);
void drawMaze();
void drawBreadCrumbs();
int isAdjacent(int space1, int space2);
int getWall(int direction, int currentSpace);
void redrawBreadCrumbs();

int main(int argc, char * argv[]) {


	cout<<endl<<"Welcome to my maze game!"<<endl;
	cout<<"To start, click and hold the space with the triangle already in it, this is the start space. "
			"Then, while holding you mouse down, drag your mouse through the maze to navigate it to the exit! "
			"Be careful! If you set a number of limited back tracks, you only get that many before you lose the game!"<<endl<<endl;

	cout<<"Sample Inputs From Command Line:"<<endl<<
			"maze"<<endl<<
			"maze <widthOfMaze> <heightOfMaze>"<<endl<<
			"maze <widthOfMaze> <heightOfMaze> <numberOfBackTracksAllowed>"<<endl;

	srand (time(NULL));

	//initialize maze to 3x3 for default
	rows = 3;
	cols = 3;
	startBackTracks = INT_MAX;

	//set current screen size
	width = 512;
	height = 512;

	//set current space and say the user hasn't won
	currentSpace = -1;
	won = false;

	//if the user enters dimensions, get them
	//if the user has also entered number of backtracks allowed, get them
	if(argc >= 4){
		rows = atoi(argv[1]);
		cols = atoi(argv[2]);
		startBackTracks = abs(atoi(argv[3]));

	}else if (argc >= 3){
		rows = atoi(argv[1]);
		cols = atoi(argv[2]);
	}

	backTracks = startBackTracks;

	//get the number of walls and spaces, then create array lengths based on them
	numSpaces = rows*cols;
	numBorders = rows*(cols+1) + (rows+1)*cols;
	borders = new border[numBorders];
	crumbs = new gridSpace[numSpaces];

	//set space between borders
	spaceBetweenBorders = 1/((double)max(rows,cols));

	//draw the maze and breadcrumbs
	drawMaze();
	drawBreadCrumbs();

	//init display window
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_RGBA );
	glutInitWindowSize( width, height );

	glutCreateWindow( "Maze" );

	//compatibility issues with the libraries and drivers installed.
#ifndef __APPLE__
	glewExperimental = GL_TRUE;
	glewInit();
#endif

	init();

	glutDisplayFunc( display );
	glutKeyboardFunc( keyboard );
	glutReshapeFunc(myReshape);
	glutMotionFunc(mouse);

	set_up_menu(myMenu);
	glutMainLoop();


	return 0;
}


//Pre: takes in the array of connections and the size of the array
//Post: returns whether or not all of the elements in the array are in 1 set
//checks to see if all the elements in the array are connected
bool inOneSet(int array[], int size){

	//only one element is allowed to have a -1 as its component
	int one = 0;
	for(int i = 0;i<size;i++){
		if (array[i] == -1 && one == 1){
			return false;
		}else if (array[i] == -1){
			one++;
		}
	}

	return true;
}


//Pre: takes in the array of connections and the starting point of which you want to find the leader
//Post: returns the value that is at the "head" of that set
//goes vertice to vertice until a vertice leads to a -1, then returns that vertice as the "head" of the set
int findLeader(int array[], int start){
	int prev = start;
	int leader = array[start];
	while (leader != -1) {
		prev = leader;
		leader = array[leader];
	}

	return prev;
}


//Pre: n/a
//Post: n/a
//init function to set attributes to the screen
void init(){

	//number of POINTS that the walls take up
	int numStandingWalls = (numBorders*2 - 4 - (numSpaces-1)*2);

	vec2 *points2 = new vec2[numStandingWalls];
	vec2 *crumbs2 = new vec2[numSpaces*3];


	int current = 0;

	//copy calculated points in from wall array to new array to be used in plotting
	for(int i = 0;i<numBorders;i++){
		if(borders[i].getIsStanding() == true){
			points2[2*current] = borders[i].getStart();
			points2[2*current+1] = borders[i].getEnd();
			current++;
		}
	}

	//copy calculated points from spaces array to new array to be plotted
	for(int i = 0;i<numSpaces;i++){
		crumbs2[3*i] = crumbs[i].getSpace1();
		crumbs2[3*i+1] = crumbs[i].getSpace2();
		crumbs2[3*i+2] = crumbs[i].getSpace3();

	}

	// Create a vertex array object
	GLuint vao[1];
#ifdef __APPLE__
	glGenVertexArraysAPPLE( 1, vao );
	glBindVertexArrayAPPLE( vao[0] );
#else
	glGenVertexArrays( 1, vao );
	glBindVertexArray( vao[0] );
#endif


	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers( 1, &buffer );
	glBindBuffer( GL_ARRAY_BUFFER, buffer );
	glBufferData( GL_ARRAY_BUFFER, numStandingWalls*sizeof(vec2) + numSpaces*3*sizeof(vec2), NULL, GL_STATIC_DRAW );

	//load the walls then the breadcrumbs
	glBufferSubData(GL_ARRAY_BUFFER,0, numStandingWalls*sizeof(vec2), points2);
	glBufferSubData(GL_ARRAY_BUFFER,numStandingWalls*sizeof(vec2), numSpaces*3*sizeof(vec2), crumbs2);

	//create and initialize element buffer
	GLuint indexBuffer;
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numSpaces*3*sizeof(GLushort),NULL,GL_DYNAMIC_DRAW);


	//draw the first breadcrumb
	GLushort *data = (GLushort*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER,GL_READ_WRITE);
	GLushort temp = vectorCrumbs.at(0)*3 + (numStandingWalls);

	data[0] = temp;
	data[1] = temp+1;
	data[2] = temp+2;


	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);


	// Load shaders and use the resulting shader program
	GLuint program = InitShader( "vshader21.glsl", "fshader21.glsl" );
	glUseProgram( program );

	projLoc = glGetUniformLocation(program,"p");
	mvLoc = glGetUniformLocation(program,"mv");

	// Initialize the vertex position attribute from the vertex shader
	GLuint loc = glGetAttribLocation( program, "vPosition" );
	glEnableVertexAttribArray( loc );
	glVertexAttribPointer( loc, 2, GL_FLOAT, GL_FALSE, 0,
			BUFFER_OFFSET(0) );

	glClearColor( 1.0, 1.0, 1.0, 1.0 ); // white background

	p = Ortho(wld_left,wld_right,wld_bot,wld_top,wld_near,wld_far);

}


//Pre: n/a
//Post: n/a
//function to display the screen
void display( void )
{
	glClear( GL_COLOR_BUFFER_BIT );     // clear the window

	//draw the walls then the breadcrumbs
	glDrawArrays( GL_LINES, 0, numBorders*2 - 4 - (numSpaces-1)*2 );
	glDrawElements(GL_TRIANGLES,vectorCrumbs.size()*3,GL_UNSIGNED_SHORT,BUFFER_OFFSET(0));

	glFlush();
}


//Pre: takes in the key that was hit, and the (x,y) location of the mouse at that time
//Post: n/acurr_wld_top
//function to detect keyboard gestures
void keyboard( unsigned char key, int x, int y )
{
	//if the escape key or q key is hit, end the program
	switch ( key ) {
	case 033:
	case 'q':
	case 'Q':
		exit( EXIT_SUCCESS );
		break;

	}
}


//Pre: passes in the x and y coordinates in pixels of the mouse on the screen
//Post: n/a
//given the points of the mouse on the screen, calculate which box the user is in and adjust the stack accordingly
void mouse(int x, int y){

	int gridPixelsX, gridPixelsY;
	double pixelsPerBox;
	int currentBlockX, currentBlockY;
	int newSpace;

	//get the number of pixels per 1 unit of measure in world coordinates
	double pixelsPerX = width/(curr_wld_right - curr_wld_left);
	double pixelsPerY = height/(curr_wld_top - curr_wld_bot);

	//get the point (0,0) from the world coordinates in pixel coordinates
	int zeroX = pixelsPerX*abs(curr_wld_left);
	int zeroY = height - pixelsPerY*abs(curr_wld_bot);

	//get the number of pixels per box in the maze
	if(min(width,height) == width){
		pixelsPerBox = pixelsPerX * spaceBetweenBorders;
	}else{
		pixelsPerBox = pixelsPerY * spaceBetweenBorders;
	}

	//get the values for the right most pixel and upper most pixel in the maze
	gridPixelsX = (pixelsPerBox*cols) + zeroX;
	gridPixelsY = zeroY - (pixelsPerBox*rows);

	//zeroX =  left most pixel of the maze
	//gridPixelsX = right most pixel of the maze
	//zeroY = bottom most pixel of the maze
	//gridPixelsY = upper most pixel of the maze


	//if the user is within the confines of the maze, get the current block they are in
	if(x>zeroX && x<gridPixelsX){
		currentBlockX = (x-zeroX)/(double)pixelsPerBox;
	}else{
		currentBlockX = -1;
	}

	if(y<zeroY && y>gridPixelsY){
		currentBlockY = (zeroY-y)/(double)pixelsPerBox;
	}else{
		currentBlockY = -1;
	}


	//if the user is not in the maze or the game is over, do nothing. If the user is in the maze, adjust the stack
	if(currentBlockY == -1 || currentBlockX == -1 || backTracks <0 || won){
		newSpace = -1;
	}else{

		//get the new space the user is in
		newSpace = currentBlockX*rows + currentBlockY;

		//make sure the new space is adjacent to the old space and that there is no wall seperating them
		int adjacency = isAdjacent(newSpace,currentSpace);
		int wall = getWall(adjacency, currentSpace);

		//if the new space is valid, adjust the stack
		if(adjacency != -1 && (!borders[wall].getIsStanding())){

			//if the newSpace is new, add it on to the stack, if not, pop the old space off of the stack
			if(newSpace != currentSpace && newSpace != prevSpace){
				prevSpace = currentSpace;
				currentSpace = newSpace;
				vectorCrumbs.push_back(currentSpace);
			}else if (newSpace != currentSpace && newSpace == prevSpace){

				currentSpace = prevSpace;

				vectorCrumbs.pop_back();
				vectorCrumbs.pop_back();

				//get the previous space
				if(vectorCrumbs.size()>0)
					prevSpace = vectorCrumbs.back();
				else
					prevSpace = startSpace;

				vectorCrumbs.push_back(currentSpace);

				//if the user entered a back trace limit, increment it
				if(startBackTracks != INT_MAX)
					backTracks--;
			}

			//redraw the bread crumbs
			redrawBreadCrumbs();

			//see if the user won based on this new move
			if(currentSpace == endSpace){
				won = true;
			}
		}
	}

	//if the user won, print the message
	//if the user lost, print another message
	if(won){
		glColor3f(0.0,1.0,0.0);
		glRasterPos2f(0.0, -0.07);
		int i = 0;
		while(winMessage[i]){
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, winMessage[i]);
			i++;
		}
	}else if(backTracks == -1){
		glColor3f(0.0,1.0,0.0);
		glRasterPos2f(0.0, -0.07);
		int i = 0;
		while(loseMessage[i]){
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, loseMessage[i]);
			i++;
		}
	}else if (startBackTracks != INT_MAX){
		char backsLeftMessage[35];
		sprintf(backsLeftMessage, "Back moves left: %d", backTracks);
		glColor3f(0.0,1.0,0.0);
		glRasterPos2f(0.0, -0.07);
		int i = 0;
		while(backsLeftMessage[i]){
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, backsLeftMessage[i]);
			i++;
		}
	}
}


//Pre: takes in which item in the menu was clicked
//Post: n/a
//performs an action based on which item in the menu was clicked
void myMenu(int item){

	//case 1: make a new maze
	//case 2: quit
	switch (item){
	case 1:{

		//get the new maze points
		drawMaze();

		//number of POINTS that the walls take up and counter for putting them into the array
		int numStandingWalls = (numBorders*2 - 4 - (numSpaces-1)*2);
		int current = 0;

		//points of the walls
		vec2 *points2 = new vec2[numStandingWalls];



		//copy calculated points in from wall array to new array to be used in plotting
		for(int i = 0;i<numBorders;i++){
			if(borders[i].getIsStanding() == true){
				points2[2*current] = borders[i].getStart();
				points2[2*current+1] = borders[i].getEnd();
				current++;
			}
		}

		//reload the buffer
		glBufferSubData(GL_ARRAY_BUFFER,0, numStandingWalls*sizeof(vec2), points2);

		//initialize the first bread crumb
		GLushort *data = (GLushort*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER,GL_READ_WRITE);
		GLushort temp = vectorCrumbs.at(0)*3 + (numStandingWalls);

		data[0] = temp;
		data[1] = temp+1;
		data[2] = temp+2;


		glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

		//redraw the frame
		glutPostRedisplay();
		break;
	}case 2:
		exit(EXIT_SUCCESS);
		break;
	}
}


//Pre: takes in the function that will handle this menu
//Post: n/a
//creates the values for the menu that will drop down on a right click
void set_up_menu(void (*myMenu)(int)){
	glutCreateMenu(myMenu);
	glutAddMenuEntry("New Game",1);
	glutAddMenuEntry("Quit",2);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}


//Pre: takes in the width and height of the screen at that time
//Post: n/a
//resizes the maze while maintaining its proportion
void myReshape(GLint w, GLint h){
	glViewport(0,0,w,h);

	width = w;
	height=  h;

	//proportionally resize the matrix
	if(w<=h){
		curr_wld_bot = wld_bot*(GLfloat)h/w;
		curr_wld_top = wld_top*(GLfloat)h/w;
	}else{
		curr_wld_left = wld_left*(GLfloat)w/h;
		curr_wld_right = wld_right*(GLfloat)w/h;
	}

	//keep the maze in the center of the screen
	p = Ortho(curr_wld_left,curr_wld_right,curr_wld_bot,curr_wld_top,curr_wld_near,curr_wld_far);

	cout<<p<<endl;
	cout<<mv<<endl;

	glUniformMatrix4fv(projLoc,1,GL_TRUE,p);
	glUniformMatrix4fv(mvLoc,1,GL_TRUE,mv);
}


//Pre: n/a
//Post: n/a
//draws a matrix based on values from global variables
void drawMaze(){

	//make sure the user has not won
	won = false;
	verticals = 0;
	backTracks = startBackTracks;

	//make all of the walls standing
	for(int i = 0;i<numBorders;i++){
		borders[i].setIsStanding(true);
	}

	//clear the stack
	while(!vectorCrumbs.empty()){
		vectorCrumbs.pop_back();
	}

	//get the vertical rows
	for(int i = 0;i<cols+1;i++){
		for(int j = 0;j<rows;j++){
			vec2 first = vec2(i*spaceBetweenBorders,j*spaceBetweenBorders);
			vec2 last = vec2(i*spaceBetweenBorders,(j*spaceBetweenBorders)+spaceBetweenBorders);
			borders[i*(rows) + j] = border(first, last);
			borders[i*(rows) + j].setIsHorizontal(false);

			if(i == 0 || i == cols){
				borders[i*(rows) + j].setIsBorder(true);
			}

			verticals++;
		}
	}

	//get the horizontal rows
	for(int i = 0;i<rows+1;i++){
		for(int j = 0;j<cols;j++){
			vec2 first = vec2(j*spaceBetweenBorders,(i*spaceBetweenBorders));
			vec2 last = vec2((j*spaceBetweenBorders)+spaceBetweenBorders,(i*spaceBetweenBorders));
			borders[(cols+1)*rows+i*(cols) + j] = border(first, last);
			borders[(cols+1)*rows+i*(cols) + j].setIsHorizontal(true);

			if(i == 0 || i == rows){
				borders[(cols+1)*rows+i*(cols) + j].setIsBorder(true);
			}
		}
	}

	//initialize the array for the disjoint sets
	int *disjointSets = new int[numSpaces];

	for(int i = 0;i<numSpaces;i++){
		disjointSets[i] = -1;
	}

	int wall;
	int space1;
	int space2;
	int aboveRow, aboveCol;
	int leader1;
	int leader2;

	//get the walls that are to be knocked down
	while(!inOneSet(disjointSets, numSpaces)){
		wall = rand()%numBorders;

		//if the wall is not already knocked down and is not a border
		if(!borders[wall].getIsBorder() && borders[wall].getIsStanding()){

			//check if the wall is horizontal or not, get the spaces the wall is seperating based on this fact
			if(!borders[wall].isHorizontal()){
				space1 = wall-rows;
				space2 = wall;
			}else{
				aboveRow = (wall-(verticals+cols))/cols;
				aboveCol = (wall-(verticals+cols))%cols;
				space1 = aboveCol*rows+aboveRow;
				space2 = space1+1;
			}

			//join the 2 sets
			leader1 = findLeader(disjointSets,space1);
			leader2 = findLeader(disjointSets,space2);

			if(!(leader1 == leader2 && leader1 != -1)){
				disjointSets[leader1] = space2;
				borders[wall].setIsStanding(false);
			}
		}
	}


	//generate the 2 openings on opposite sides of the maze
	//start opening
	wall = rand()%rows;

	currentSpace = wall;
	prevSpace = wall;
	startSpace = wall;

	if(borders[wall].getIsBorder() && borders[wall].getIsStanding()){
		borders[wall].setIsStanding(false);
	}

	//add first crumb position to the stack
	vectorCrumbs.push_back(wall);

	//end opening
	wall = rand()%rows + (numSpaces);

	endSpace = wall-rows;

	if(borders[wall].getIsBorder() && borders[wall].getIsStanding()){
		borders[wall].setIsStanding(false);
	}
}


//Pre: n/a
//Post: n/a
//generates the points for the bread crumbs in order
void drawBreadCrumbs(){
	for(int i = 0;i<cols;i++){
		for(int j = 0;j<rows;j++){
			GLfloat x = i*spaceBetweenBorders + spaceBetweenBorders/2;
			GLfloat y = (j*spaceBetweenBorders) + spaceBetweenBorders/2;

			//create the first point of the bred crumb, the other 2 points will be generated in the setCrumb() function
			vec2 point1 = vec2(x,y);
			crumbs[i*rows+j].setCrumb(point1, spaceBetweenBorders);
		}
	}
}


//Pre: passes in 2 spaces in the maze
//Post: returns an integer corresponding to which direction space 2 is to space 1
//sees if the 2 spaces are adjacent, if they are, return an integer to show which direction space2 is in according to space 1
//if they are not adjacent, return -1
int isAdjacent(int space1, int space2){
	//above
	if(space2 == space1+1 && space2%rows != 0){
		return 1;
	}

	//below
	if(space2 == space1-1 && space1%rows != 0){
		return 2;
	}

	//right
	if(space2 == space1-rows){
		return 3;
	}

	//left
	if(space2 == space1+rows){
		return 4;
	}

	return -1;
}


//Pre: passes in a direction the user is moving and the current space the user is in
//Post: returns which wall borders that current space in the given direction
//calculates which wall is being moved to based on the space the user is currently in and direction they are moving
int getWall(int direction, int currentSpace){
	int wall = -1;

	//1 = up, 2 = down, 3 = right, 4 = left
	if(direction == 1){
		wall = verticals + (currentSpace%rows)*cols + currentSpace/rows;
	}else if (direction == 2){
		wall = verticals + cols + (currentSpace%rows)*cols + currentSpace/rows;
	}else if (direction == 3){
		wall = currentSpace + rows;
	}else if (direction == 4){
		wall = currentSpace;
	}

	return wall;
}


//Pre: n/a
//Post: n/a
//reloads the element array buffer to redraw the breadcrumbs with every step
void redrawBreadCrumbs(){

	//number of POINTS that the walls take up
	int numStandingWalls = (numBorders*2 - 4 - (numSpaces-1)*2);

	//refresh the buffer by looping through the current spaces in the stack
	GLushort *data = (GLushort*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER,GL_READ_WRITE);

	for(int i = 0;i<(int)vectorCrumbs.size();i++){
		GLushort temp = vectorCrumbs.at(i)*3 + (numStandingWalls);

		data[3*i] = temp;
		data[3*i+1] = temp+1;
		data[3*i+2] = temp+2;
	}


	//unmap and redraw the scene
	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
	glutPostRedisplay();
}

