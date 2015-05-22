/*
 * gridSpace.h
 *
 *  Created on: Feb 3, 2015
 *      Author: Nick Zayatz
 */

#ifndef GRIDSPACE_H_
#define GRIDSPACE_H_
#include "../include/Angel.h"

namespace std {

class gridSpace {
public:
	gridSpace();

	//sets and gets for variables
	void setCrumb(vec2 space1, GLfloat distanceBetweenWalls);

	vec2 getSpace1(){
		return crumbSpace1;
	}

	vec2 getSpace2(){
		return crumbSpace2;
	}

	vec2 getSpace3(){
		return crumbSpace3;
	}

	void setCrumbVisible(bool maybe){
		isCrumbVisible = maybe;
	}

	bool getCrumbVisible(){
		return isCrumbVisible;
	}

	virtual ~gridSpace();

private:
	//3 points of the crumb
	vec2 crumbSpace1;
	vec2 crumbSpace2;
	vec2 crumbSpace3;

	//is the crumb shown
	bool isCrumbVisible;
};

} /* namespace std */

#endif /* GRIDSPACE_H_ */
