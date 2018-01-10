#pragma once

#include "Globals.h"
#include "Renderer.h"
#include <fstream>

struct memLoc
{
	uint64_t offset;
	uint64_t size;
};

class RenderObject
{
public:
//	RenderObject(ObjectID global_id, Renderer* renderer) {}
	RenderObject() {}

	std::string m_dataFile;

	Renderer* p_renderer;
	memLoc m_vertex;
	memLoc m_index;
	memLoc m_texture;
	memLoc m_uniform;
	
	//Data layout
	//	Byte 1: Flags: Tell the scene which arrays need to be filled
	//		Flags: {Vertex, Index, Texture, Uniform, Unused, Unused, Unused, Unused}
	//	2 Byte Segments: Size of each buffer
	//	Null terminated sections of data, in the same layout as the flags.
	virtual byte* getData() = 0;
	virtual byte draw(VkBuffer vertexBuffer, VkBuffer indexBuffer, VkBuffer textureBuffer, VkBuffer uniformBuffer, int queueIndex, byte* data) = 0;
	virtual VkPipeline getPipeline() = 0;

	ObjectID id;
};

VkShaderModule loadShaderFromFile(std::string shaderFile, VkDevice device)
{
	std::ifstream shaderStream(shaderFile, std::ios::ate | std::ios::binary);
	if (!shaderStream.is_open())
		return VK_NULL_HANDLE;
	uint32_t fileSize = (uint32_t)shaderStream.tellg();
	std::vector<char> buffer(fileSize);
	shaderStream.seekg(0);
	shaderStream.read(buffer.data(), fileSize);
	shaderStream.close();
	
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = buffer.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(buffer.data());

	VkShaderModule shaderModule;
	VkResult result = vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule);
	return shaderModule;
}

//Example Object
class Cube_R : private RenderObject
{
public:
	Cube_R(glm::vec3 color, Renderer* renderer)
	{
		p_renderer = renderer;
		m_color = color;
		id.global_id = 0x0001000000000001;
		id.instance_id = 0x0100000000000000;
		if (m_pipeline == VK_NULL_HANDLE)
		{
			//Initialize the Pipeline
			VkShaderModule vertexShader = loadShaderFromFile("cube_vert.spv", p_renderer->device);
			VkShaderModule fragmentShader = loadShaderFromFile("cube_frag.spv", p_renderer->device);
			
			if (vertexShader == VK_NULL_HANDLE || fragmentShader == VK_NULL_HANDLE)
				throw "failed to create shaders";

			VkPipelineShaderStageCreateInfo shaderInfo[2] = { {},{} };

			shaderInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shaderInfo[0].flags = 0;
			shaderInfo[0].pNext = NULL;
			shaderInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
			shaderInfo[0].module = vertexShader;
			shaderInfo[0].pName = "main";

			shaderInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shaderInfo[1].flags = 0;
			shaderInfo[1].pNext = NULL;
			shaderInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			shaderInfo[1].module = fragmentShader;
			shaderInfo[1].pName = "main";

			VkVertexInputBindingDescription bindingDescription = {};
			bindingDescription.binding = 0;
			bindingDescription.stride = 2 * sizeof(glm::vec3);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			VkVertexInputAttributeDescription attributeDescriptions[2] = { {},{} };
			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[0].offset = 0 * sizeof(glm::vec3);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = 1 * sizeof(glm::vec3);

			VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
			vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			vertexInputInfo.flags = 0;
			vertexInputInfo.pNext = NULL;
			vertexInputInfo.vertexBindingDescriptionCount = 1;
			vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
			vertexInputInfo.vertexAttributeDescriptionCount = 2;
			vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions;

			VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
			inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			inputAssembly.flags = 0;
			inputAssembly.pNext = NULL;
			inputAssembly.primitiveRestartEnable = VK_FALSE;

			VkViewport viewport = {};
			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = (float)renderer->swapchainExtent.width;
			viewport.height = (float)renderer->swapchainExtent.height;
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;

			VkRect2D scissor = {};
			scissor.offset = { 0, 0 };
			scissor.extent = renderer->swapchainExtent;

			VkPipelineViewportStateCreateInfo viewportState = {};
			viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			viewportState.pNext = NULL;
			viewportState.flags = 0;
			viewportState.pScissors = &scissor;
			viewportState.pViewports = &viewport;
			viewportState.scissorCount = 1;
			viewportState.viewportCount = 1;

			VkPipelineRasterizationStateCreateInfo rasterizer = {};
			rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			rasterizer.depthClampEnable = VK_FALSE;
			rasterizer.rasterizerDiscardEnable = VK_FALSE;
			rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
			rasterizer.lineWidth = 1.0f;
			rasterizer.cullMode = VK_CULL_MODE_NONE;
			rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
			rasterizer.depthBiasEnable = VK_FALSE;
			rasterizer.depthBiasConstantFactor = 0.0f;
			rasterizer.depthBiasClamp = 0.0f;
			rasterizer.depthBiasSlopeFactor = 0.0f;

			VkPipelineMultisampleStateCreateInfo multisampling = {};
			multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			multisampling.sampleShadingEnable = VK_FALSE;
			multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
			multisampling.minSampleShading = 1.0f;
			multisampling.pSampleMask = nullptr;
			multisampling.alphaToCoverageEnable = VK_FALSE;
			multisampling.alphaToOneEnable = VK_FALSE;

			VkPipelineDepthStencilStateCreateInfo stencilInfo = {};

			VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
			colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
			colorBlendAttachment.blendEnable = VK_FALSE;
			colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
			colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
			colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
			colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
			colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
			colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

			VkPipelineColorBlendStateCreateInfo colorBlending = {};
			colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			colorBlending.logicOpEnable = VK_FALSE;
			colorBlending.logicOp = VK_LOGIC_OP_COPY;
			colorBlending.attachmentCount = 1;
			colorBlending.pAttachments = &colorBlendAttachment;
			colorBlending.blendConstants[0] = 0.0f;
			colorBlending.blendConstants[1] = 0.0f;
			colorBlending.blendConstants[2] = 0.0f;
			colorBlending.blendConstants[3] = 0.0f;

			VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_LINE_WIDTH };
			VkPipelineDynamicStateCreateInfo dynamicState = {};
			dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
			dynamicState.dynamicStateCount = 2;
			dynamicState.pDynamicStates = dynamicStates;
			
			VkDescriptorSetLayoutBinding uboLayoutBinding = {};
			uboLayoutBinding.binding = 0;
			uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			uboLayoutBinding.descriptorCount = 1;
			uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
			uboLayoutBinding.pImmutableSamplers = nullptr;

			VkDescriptorSetLayoutCreateInfo layoutInfo = {};
			layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			layoutInfo.bindingCount = 1;
			layoutInfo.flags = 0;
			layoutInfo.pBindings = &uboLayoutBinding;
			vkCreateDescriptorSetLayout(renderer->device, &layoutInfo, nullptr, &m_descriptorLayout);

			VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
			pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			pipelineLayoutInfo.setLayoutCount = 1;
			pipelineLayoutInfo.pSetLayouts = &m_descriptorLayout;

			vkCreatePipelineLayout(renderer->device, &pipelineLayoutInfo, nullptr, &m_pipelineLayout);

			VkGraphicsPipelineCreateInfo pipelineInfo = {};
			pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			pipelineInfo.stageCount = 2;
			pipelineInfo.pStages = shaderInfo;
			pipelineInfo.pVertexInputState = &vertexInputInfo;
			pipelineInfo.pInputAssemblyState = &inputAssembly;
			pipelineInfo.pViewportState = &viewportState;
			pipelineInfo.pRasterizationState = &rasterizer;
			pipelineInfo.pMultisampleState = &multisampling;
			pipelineInfo.pDepthStencilState = nullptr;
			pipelineInfo.pColorBlendState = &colorBlending;
			pipelineInfo.pDynamicState = &dynamicState;
			pipelineInfo.layout = m_pipelineLayout;
			pipelineInfo.renderPass = renderer->renderPass;
			pipelineInfo.subpass = 0;
			pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
			pipelineInfo.basePipelineIndex = -1;

			vkCreateGraphicsPipelines(renderer->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_pipeline);

			vkDestroyShaderModule(renderer->device, fragmentShader, nullptr);
			vkDestroyShaderModule(renderer->device, vertexShader, nullptr);

			VkDescriptorSetLayout layouts[] = { m_descriptorLayout };
			VkDescriptorSetAllocateInfo allocInfo = {};
			allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocInfo.descriptorPool = renderer->descriptorPool;
			allocInfo.descriptorSetCount = 1;
			allocInfo.pSetLayouts = layouts;

			vkAllocateDescriptorSets(renderer->device, &allocInfo, &m_descriptorSet);
		}
	}

	byte* getData()
	{
		byte header = 0xC0;
		byte vertexSize = 8 * sizeof(glm::vec3) * 2;
		byte indexSize = 12 * sizeof(vecu32);
		byte uniformSize = sizeof(UniformBufferObject);
		byte* data = new byte[4 + vertexSize + indexSize];
		data[0] = header;
		data[1] = vertexSize;
		data[2] = indexSize;
		data[3] = uniformSize;
		((glm::vec3*)&data[4])[0] = glm::vec3( 1,  1,  1);
		((glm::vec3*)&data[4])[1] = m_color;
		((glm::vec3*)&data[4])[2] = glm::vec3( 1, -1,  1);
		((glm::vec3*)&data[4])[3] = m_color;
		((glm::vec3*)&data[4])[4] = glm::vec3(-1, -1,  1);
		((glm::vec3*)&data[4])[5] = m_color;
		((glm::vec3*)&data[4])[6] = glm::vec3(-1,  1,  1);
		((glm::vec3*)&data[4])[7] = m_color;
		((glm::vec3*)&data[4])[8] = glm::vec3( 1,  1, -1);
		((glm::vec3*)&data[4])[9] = m_color;
		((glm::vec3*)&data[4])[10] = glm::vec3( 1, -1, -1);
		((glm::vec3*)&data[4])[11] = m_color;
		((glm::vec3*)&data[4])[12] = glm::vec3(-1, -1, -1);
		((glm::vec3*)&data[4])[13] = m_color;
		((glm::vec3*)&data[4])[14] = glm::vec3(-1,  1, -1);
		((glm::vec3*)&data[4])[15] = m_color;

		((vecu32*)&((glm::vec3*)&data[5])[16])[0]  = { 0, 1, 2 };
		((vecu32*)&((glm::vec3*)&data[5])[16])[1]  = { 2, 3, 0 };
		((vecu32*)&((glm::vec3*)&data[5])[16])[2]  = { 0, 3, 7 };
		((vecu32*)&((glm::vec3*)&data[5])[16])[3]  = { 7, 4, 0 };
		((vecu32*)&((glm::vec3*)&data[5])[16])[4]  = { 0, 4, 5 };
		((vecu32*)&((glm::vec3*)&data[5])[16])[5]  = { 5, 1, 0 };
		((vecu32*)&((glm::vec3*)&data[5])[16])[6]  = { 1, 5, 6 };
		((vecu32*)&((glm::vec3*)&data[5])[16])[7]  = { 6, 2, 1 };
		((vecu32*)&((glm::vec3*)&data[5])[16])[8]  = { 6, 2, 3 };
		((vecu32*)&((glm::vec3*)&data[5])[16])[9]  = { 7, 6, 2 };
		((vecu32*)&((glm::vec3*)&data[5])[16])[10] = { 7, 4, 5 };
		((vecu32*)&((glm::vec3*)&data[5])[16])[11] = { 5, 6, 7 };

		return data;
	}


	//Data layout
	byte draw(VkBuffer vertexBuffer, VkBuffer indexBuffer, VkBuffer textureBuffer, VkBuffer uniformBuffer, int queueIndex, byte* data)
	{
		if (m_drawBuffer == VK_NULL_HANDLE)
		{
			VkCommandBufferAllocateInfo allocInfo = {};
			allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocInfo.commandPool = p_renderer->commandPool;
			allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocInfo.commandBufferCount = 1;
			
			vkAllocateCommandBuffers(p_renderer->device, &allocInfo, &m_drawBuffer);

			VkCommandBufferBeginInfo beginInfo = {};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
			beginInfo.pInheritanceInfo = nullptr;

			vkBeginCommandBuffer(m_drawBuffer, &beginInfo);

			vkCmdBindDescriptorSets(m_drawBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 1, &m_descriptorSet, 0, nullptr);

			vkCmdBindPipeline(m_drawBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
			
			VkBuffer vertexBuffers[] = { vertexBuffer };
			VkDeviceSize offsets[] = { m_vertex.offset };

			vkCmdBindVertexBuffers(m_drawBuffer, 0, 1, vertexBuffers, offsets);
			vkCmdBindIndexBuffer(m_drawBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);

			vkCmdDrawIndexed(m_drawBuffer, m_index.size / sizeof(uint32_t), 1, 0, 0, 0);
			vkEndCommandBuffer(m_drawBuffer);
		}

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { p_renderer->imageAvailableSemaphore };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_drawBuffer;

		vkQueueSubmit(p_renderer->graphicsQueues[queueIndex], 1, &submitInfo, VK_NULL_HANDLE);
		return 0x00;
	}

	VkPipeline getPipeline()
	{
		return m_pipeline;
	}

	glm::vec3 m_color;
	static VkPipeline m_pipeline;
	static VkDescriptorSetLayout m_descriptorLayout;
	static VkPipelineLayout m_pipelineLayout;
	static VkCommandBuffer m_drawBuffer;
	static VkDescriptorSet m_descriptorSet;
};

VkPipeline Cube_R::m_pipeline = VK_NULL_HANDLE;
VkDescriptorSetLayout Cube_R::m_descriptorLayout = VK_NULL_HANDLE;
VkPipelineLayout Cube_R::m_pipelineLayout = VK_NULL_HANDLE;
VkCommandBuffer Cube_R::m_drawBuffer = VK_NULL_HANDLE;
VkDescriptorSet Cube_R::m_descriptorSet = VK_NULL_HANDLE;
