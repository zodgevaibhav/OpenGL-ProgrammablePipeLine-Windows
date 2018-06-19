#pragma once
#include "VertexSkinData.h"
class SkinningData
{
public:

	char *jointOrder[16] = { "Torso","Chest","Neck","Head","Upper_Arm_L","Lower_Arm_L","Hand_L","Upper_Arm_R","Lower_Arm_R","Hand_R","Upper_Leg_L","Lower_Leg_L","Foot_L","Upper_Leg_R","Lower_Leg_R","Foot_R" };
	VertexSkinData verticesSkinData[10];

	SkinningData();
	~SkinningData();
};

