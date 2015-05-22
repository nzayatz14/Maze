/*
 * border.h
 *
 *  Created on: Feb 3, 2015
 *      Author: Nick Zayatz
 */

#ifndef BORDER_H_
#define BORDER_H_
#include "../include/Angel.h"

namespace std {

class border {
public:

	border();
	border(vec2 first, vec2 last);
	virtual ~border();

	//set and get methods
	void setStart(vec2 Start);
	vec2 getStart();

	void setEnd(vec2 End);
	vec2 getEnd();

	bool isHorizontal(){
		return horizontal;
	}

	void setIsHorizontal(bool maybe){
		horizontal = maybe;
	}

	bool getIsBorder(){
		return isBorder;
	}

	void setIsBorder(bool maybe){
		isBorder = maybe;
	}

	bool getIsStanding(){
		return isStanding;
	}

	void setIsStanding(bool maybe){
		isStanding = maybe;
	}

private:
	vec2 start;
	vec2 end;
	bool horizontal;
	bool isBorder;
	bool isStanding;

};

} /* namespace std */

#endif /* BORDER_H_ */
