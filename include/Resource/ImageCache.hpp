#ifndef RESOURCE_IMAGE_CACHE_HPP
#define RESOURCE_IMAGE_CACHE_HPP

#include "UGO_pch.hpp"

namespace UGO::Resource {

    class ImageCache {
    public:
        static std::shared_ptr<Util::Image> GetImage(const std::string& path);

    private:
        static std::unordered_map<std::string, std::shared_ptr<Util::Image>> s_Cache;
    };

} // namespace UGO::Resource

#endif // RESOURCE_IMAGE_CACHE_HPP
