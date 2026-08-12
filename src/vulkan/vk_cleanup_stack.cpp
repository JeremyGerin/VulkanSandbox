#include "vulkan/vk_cleanup_stack.hpp"

void unwind(CleanupStack& stack) {
    for (auto it = stack.rbegin(); it != stack.rend(); ++it) {
        (*it)();
    }
    stack.clear();
}