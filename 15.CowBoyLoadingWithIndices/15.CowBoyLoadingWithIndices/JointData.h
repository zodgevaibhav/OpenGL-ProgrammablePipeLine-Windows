#pragma once

#include "vmath.h"

class JointData
{
public:
	JointData();
	JointData(int , char* , vmath::mat4 );
	int index;
	char* nameId;
	vmath::mat4 bindLocalTransform;

	~JointData();
};

