#pragma once
#include "PixieEngineCoreHeaders.h"

#define MAX_BONES_PER_VERTEX 4

struct Vertex {
	glm::vec3 p;
	glm::vec3 n;
	glm::vec2 uv;
	int32_t boneIDs[MAX_BONES_PER_VERTEX];
	float boneWeights[MAX_BONES_PER_VERTEX];
	Vertex(const glm::vec3& _p = glm::vec3(0), const glm::vec3& _n = glm::vec3(0), const glm::vec2& _uv = glm::vec2(0));
};
