#pragma once

namespace prism
{
  class CommandBuffer;
  class Fence;

  class Queue
  {
  public:
    Queue(VkQueue queue, uint32_t family_index, uint32_t index);

    Queue(const Queue &) = default;

    Queue(Queue &&other) noexcept;

    ~Queue();

    Queue &operator=(const Queue &) = delete;

    Queue &operator=(Queue &&) = delete;

    VkQueue get_handle() const;

    uint32_t get_family_index() const;

    uint32_t get_index() const;

    const VkQueueFamilyProperties &get_properties() const;

    VkBool32 support_present() const;

    void submit(const CommandBuffer &cmd_buffer, VkFence fence = VK_NULL_HANDLE) const;

    void submit(const VkSubmitInfo& info, const Fence& fence) const;

    VkResult present(const VkPresentInfoKHR &present_info) const;

    void wait_idle() const;

  private:
    VkQueue m_handle;

    uint32_t m_family_index;

    uint32_t m_index;

    VkBool32 m_can_present;

    VkQueueFamilyProperties m_properties;

  }; // class Queue
} // namespace prism