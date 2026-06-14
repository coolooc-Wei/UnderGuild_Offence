#ifndef UGO_PCH_HPP
#define UGO_PCH_HPP

// standard includes
#include <algorithm>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <iostream>
#include <cmath>
#include <cassert>
#include <unordered_set>
#include <algorithm>
#include <queue>
#include <fstream>
#include <random>
#include <unordered_map>
#include <glm/geometric.hpp>
#include <functional>
#include <optional>

// PTSD includes
#include "Util/GameObject.hpp"
#include "Util/Text.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Renderer.hpp"
#include "Util/Color.hpp"
#include "Util/Logger.hpp"

// Define UGO_DISABLE_LOG_DEBUG (e.g. in CMakeLists.txt) to silence LOG_DEBUG.
#ifdef UGO_DISABLE_LOG_DEBUG
#undef LOG_DEBUG
#define LOG_DEBUG(...) (void)0
#endif

#include "Util/Time.hpp"
#include "Util/Image.hpp"
#include "Util/Animation.hpp"
#include "Core/Drawable.hpp"

// Third-party includes
#include <nlohmann/json.hpp>

// Mathematical Tools
#include <glm/glm.hpp>

#endif // UGO_PCH_HPP
