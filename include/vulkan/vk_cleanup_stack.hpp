#pragma once

#include <vector>
#include <functional>

using CleanupStack = std::vector<std::function<void()>>;

// Execute les cleanups en ordre inverse (LIFO), comme des destructeurs
void unwind(CleanupStack& stack);
