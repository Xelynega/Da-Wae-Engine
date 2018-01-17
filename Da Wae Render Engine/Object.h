#pragma once

#include "RenderObject.h"
#include "glm\glm.hpp"
#include "glm\gtc\quaternion.hpp"

class Object
{
	Object(ObjectID id);
	//Added this when I was tired and don't remember why, probably a good reason though.
	//Object(byte addonID, uint64_t global_id);
	RenderObject* m_renderObject;
	ObjectID id;
	glm::mat4 position;
	glm::mat4 scale;
	glm::quat rotation;
};