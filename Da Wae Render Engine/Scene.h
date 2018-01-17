#pragma once

#include "Object.h"
#include <vulkan\vulkan.hpp>

class Scene
{
public:
	Scene();
	//Object*		m_objects;
	//VkBuffer	m_vertexData;
	//VkBuffer	m_indexData;
	//VkBuffer	m_textureData;
	//VkBuffer	m_uniformData;

	//ObjectID loadObject(ObjectID global_id);
	//void optimizeObjectList();
	//bool deleteObject(ObjectID instance_id);
	void update(Renderer* renderer);
};

Scene::Scene()
{

}

void Scene::update(Renderer* renderer)
{
	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(renderer->device, renderer->swapchain, UINT64_MAX, renderer->imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

	if (result != VK_SUCCESS)
		return;

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &renderer->imageAvailableSemaphore;

	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &renderer->renderFinishedSemaphore;

	VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
	submitInfo.pWaitDstStageMask = &waitDstStageMask;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &renderer->commandBuffers[imageIndex];

	result = vkQueueSubmit(renderer->presentQueue, 1, &submitInfo, VK_NULL_HANDLE);
	if (result != VK_SUCCESS)
		return;
	
	//////////////////////////////////////
	//Draw all objects in the scene here//
	//////////////////////////////////////

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &renderer->renderFinishedSemaphore;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &renderer->swapchain;
	presentInfo.pImageIndices = &imageIndex;

	result = vkQueuePresentKHR(renderer->presentQueue, &presentInfo);

	if (result != VK_SUCCESS)
		return;
}