#include "prism/vulkan/queue.h"

#include "prism/vulkan/error.h"
#include "prism/vulkan/command_buffer.h"

using namespace prism;

Queue::Queue(VkQueue queue, uint32_t family_index, uint32_t index)
		: m_handle(queue), m_family_index(family_index), m_index(index)
{
}

Queue::Queue(Queue &&other) noexcept
		: m_handle(std::exchange(other.m_handle, nullptr)),
		  m_family_index(std::exchange(other.m_family_index, 0)),
		  m_index(std::exchange(other.m_index, 0)),
		  m_can_present(std::exchange(other.m_can_present, VK_FALSE)),
		  m_properties(std::exchange(other.m_properties, {}))
{
}

Queue::~Queue()
{
}

VkQueue Queue::get_handle() const
{
	return m_handle;
}

uint32_t Queue::get_family_index() const
{
	return m_family_index;
}

uint32_t Queue::get_index() const
{
	return m_index;
}

const VkQueueFamilyProperties &Queue::get_properties() const
{
	return m_properties;
}

VkBool32 Queue::support_present() const
{
	return m_can_present;
}

void Queue::submit(const CommandBuffer &cmd_buffer, VkFence fence) const
{
	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &cmd_buffer.get_handle();
	
	VK_CHECK(vkQueueSubmit(m_handle, 1, &submit_info, fence));
}

void Queue::present(const VkPresentInfoKHR &present_info) const
{
	VK_CHECK(vkQueuePresentKHR(m_handle, &present_info));
}

void Queue::wait_idle() const
{
	VK_CHECK(vkQueueWaitIdle(m_handle));
}