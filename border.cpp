/*
 * border.cpp
 *
 *  Created on: Feb 3, 2015
 *      Author: Nick Zayatz
 */

#include "border.h"

namespace std {

border::border() {
	// TODO Auto-generated constructor stub
	horizontal = false;
	isBorder = false;
	isStanding = true;
}


//constructor with start and end points included
border::border(vec2 first, vec2 last){
	start = first;
	end = last;
	horizontal = false;
	isBorder = false;
	isStanding = true;
}

border::~border() {
	// TODO Auto-generated destructor stub
}

//setter and getter methods
void border::setStart(vec2 Start){
	start=  Start;
}

vec2 border::getStart(){
	return start;
}

void border::setEnd(vec2 End){
	end = End;
}

vec2 border::getEnd(){
	return end;
}

} /* namespace std */
