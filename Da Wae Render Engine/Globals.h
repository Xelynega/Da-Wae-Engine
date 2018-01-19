#pragma once

#include "vulkan\vulkan.hpp"
#include "glm\glm.hpp"

#define DESCRIPTOR_SET_COUNT 100

/*
Global ID:
Byte 1-2: Addon ID
	0x00: Primitives/Test types
	0x01: Base Addon
Byte 3: Object Type
	0x00: Error Type
	0x01: Render Object
	0x02: Static Object
Byte 4: Flags:
	0b00000001: Parity Bit (Sum of all flag bits)
	0b00000010:
	0b00000100:
	0b00001000:
	0b00010000:
	0b00100000:
	0b01000000:
	0b10000000:
Byte 5-8: Object ID
	0x00: Error ID

Instance ID:
Byte 1: Type ID
	0x00: Error Type
	0x01: Render Object
Byte 2-8: Sequentially Assigned IDs
	0x00: Error ID Except for uninstanced objects (ie RenderObject)
*/
 
typedef unsigned char byte; 
 
struct ObjectID
{
	uint64_t global_id;
	uint64_t instance_id;
	ObjectID(uint64_t id) : global_id(id), instance_id(0) {}
	ObjectID(uint64_t g_id, uint64_t i_id) : global_id(g_id), instance_id(i_id) {}
	ObjectID(uint64_t g_id, uint64_t i_id, uint8_t t_id) : global_id(g_id), instance_id(i_id) {}
	ObjectID() :global_id(0), instance_id(0) {}
};

struct vecu32
{
	uint32_t x, y, z;
	vecu32(uint32_t nx, uint32_t ny, uint32_t nz) :x(nx), y(ny), z(nz) {}
};

struct UniformBufferObject
{
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
};

struct memLoc
{
	uint64_t offset;
	uint64_t size;
};

struct DescriptorData
{
	VkDescriptorSet descriptorSet;
	VkBuffer uniformBuffer;
	uint64_t offset;
	uint64_t size;
};