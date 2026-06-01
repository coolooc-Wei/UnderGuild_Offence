#ifndef UGO_CORE_HASH_HPP
#define UGO_CORE_HASH_HPP

#include "UGO_pch.hpp"


namespace UGO {
namespace Core {

    struct Ivec2Hash {
        std::size_t operator()(const glm::ivec2& v) const {
            using std::size_t;
            using std::string;
            using std::hash;

            size_t h1 = hash<int>()(v.x);
            size_t h2 = hash<int>()(v.y);

            // 64 bits sys
            if constexpr(sizeof(void*) == 8) { return h1 ^ (h2 << 32 | h2 >> 32); }
            // 32 bits sys
            else { return  h1 ^ (h2 << 16 | h2 >> 16); }
        }
    };

} // namespace Core
} // namespace UGO

#endif // UGO_CORE_HASH_HPP
