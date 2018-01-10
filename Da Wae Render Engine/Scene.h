#pragma once

#include "Object.h"
#include <vulkan\vulkan.hpp>

class Scene
{
	Object*		m_objects;
	VkBuffer	m_vertexData;
	VkBuffer	m_indexData;
	VkBuffer	m_textureData;
	VkBuffer	m_uniformData;

	ObjectID loadObject(ObjectID global_id);
	void optimizeObjectList();
	bool deleteObject(ObjectID instance_id);
	void update();
};