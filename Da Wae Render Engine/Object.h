#pragma once

#include "RenderObject.h"
#include "glm\glm.hpp"
#include "glm\gtc\quaternion.hpp"

class Object
{
	Object(ObjectID id);
	Object(byte modID, uint64_t global_id);
	RenderObject* m_renderObject;
	ObjectID id;
	glm::mat4 position;
	glm::mat4 scale;
	glm::quat rotation;
};