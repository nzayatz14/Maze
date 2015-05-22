/*
 * gridSpace.cpp
 *
 *  Created on: Feb 3, 2015
 *      Author: Nick Zayatz
 */

#include "gridSpace.h"
#define breadCrumbSize .2

namespace std {

gridSpace::gridSpace() {
	//the crumb is visible by default
	isCrumbVisible=  true;

}

gridSpace::~gridSpace() {
	// TODO Auto-generated destructor stub
}


//Pre: passes in the point of the first crumb and the distance between walls
//Post: n/a
//creates the crumb, setting the first point to the value that was passed in and the other values
//based on the size of the grid spaces
void gridSpace::setCrumb(vec2 space1, GLfloat distanceBetweenWalls){
	crumbSpace1 = space1;

	crumbSpace2.x = space1.x-(distanceBetweenWalls*breadCrumbSize)/2;
	crumbSpace2.y = space1.y-(distanceBetweenWalls*breadCrumbSize*sqrt(3))/2;

	crumbSpace3.x = space1.x+(distanceBetweenWalls*breadCrumbSize)/2;
	crumbSpace3.y = space1.y-(distanceBetweenWalls*breadCrumbSize*sqrt(3))/2;
}

} /* namespace std */
