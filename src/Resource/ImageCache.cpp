#include "Resource/ImageCache.hpp"

namespace UGO::Resource {

std::unordered_map<std::string, std::shared_ptr<Util::Image>> ImageCache::s_Cache;

std::shared_ptr<Util::Image> ImageCache::GetImage(const std::string& path) {
    auto& cached = s_Cache[path];
    if (!cached) { cached = std::make_shared<Util::Image>(path); }
    return cached;
}

} // namespace UGO::Resource
