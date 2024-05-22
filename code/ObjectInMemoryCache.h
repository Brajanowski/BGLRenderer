#pragma once

#include "Base.h"

#include <memory>
#include <unordered_map>

namespace BGLRenderer
{
    template<typename TKey, class TObject>
    class ObjectInMemoryCache
    {
    public:
        ObjectInMemoryCache() = default;
        ~ObjectInMemoryCache() = default;

        inline void set(const TKey& key, const std::shared_ptr<TObject>& obj) { _map[key] = obj; }
        
        inline std::shared_ptr<TObject> get(const TKey& key)
        {
            if (exists(key))
            {
                return _map[key];
            }

            return nullptr;
        }

        inline bool exists(const TKey& key) { return _map.contains(key); }

    private:
        std::unordered_map<TKey, std::shared_ptr<TObject>> _map;
    };
}
