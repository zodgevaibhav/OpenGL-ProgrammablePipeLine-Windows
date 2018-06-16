#pragma once
#include "JointData.h"
class SkeletonData
{
public:
	SkeletonData();

	int jointData;
	JointData headJoint;

	SkeletonData(int, JointData);

	~SkeletonData();
};

