#pragma once

#include "MeshData.h"


	
MeshData::MeshData()
{

}

int *MeshData::getJointIds()
	{
		return jointIds;
	}

	 float *MeshData::getVertexWeights() {
		return vertexWeights;
	}

	float *MeshData::getVertices() {
		return vertices;
	}

	float* MeshData::getTextureCoords() {
		return textureCoords;
	}

	float* MeshData::getNormals() {
		return normals;
	}

	int* MeshData::getIndices() {
		return indices;
	}

	int MeshData::getVertexCount() {
		return (sizeof(vertices)/sizeof(float))/ DIMENSIONS;
	}

	MeshData::~MeshData()
	{
	}

