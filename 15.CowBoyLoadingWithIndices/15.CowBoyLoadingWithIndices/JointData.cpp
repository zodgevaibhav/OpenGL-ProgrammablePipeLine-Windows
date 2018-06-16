#include "JointData.h"


JointData::JointData()
{
}

JointData::JointData(int lIndex, char *lNameId, vmath::mat4 lBindLocalTransform)
{
	index = lIndex;
	nameId = lNameId;
	bindLocalTransform = lBindLocalTransform;
}

//********************** To be implemented ********************

//public final List<JointData> children = new ArrayList<JointData>();
//public void addChild(JointData child) {
//	children.add(child);
//}


JointData::~JointData()
{
}
