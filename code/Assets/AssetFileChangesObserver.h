#pragma once

#include <functional>
#include <filesystem>

#include "AssetContentLoader.h"

#include <Foundation/Publisher.h>
#include <Foundation/Timer.h>

namespace BGLRenderer
{
    struct AssetFileChangedEvent
    {
        std::filesystem::path path;
        std::filesystem::file_time_type writeTime;
    };

    class AssetFileChangesObserver
    {
    public:
        AssetFileChangesObserver(const std::shared_ptr<AssetContentLoader>& contentLoader, float checkDelay = 0.5f);
        ~AssetFileChangesObserver() = default;

        void tick();

        Publisher<AssetFileChangedEvent>::ListenerHandle listenFileChanged(const std::filesystem::path& path, const Publisher<AssetFileChangedEvent>::CallbackFn& callback);
        void removeFileChangedListener(const std::filesystem::path& path, Publisher<AssetFileChangedEvent>::ListenerHandle handle);

    private:
        std::shared_ptr<AssetContentLoader> _contentLoader;

        HighResolutionTimer _checkTimer;
        float _checkDelay = 0.5f;

        struct ObservedAssetFile
        {
            std::filesystem::path path;
            std::filesystem::file_time_type lastWriteTime;
            Publisher<AssetFileChangedEvent> publisher;
        };
        
        std::unordered_map<std::filesystem::path, ObservedAssetFile> _observedFilesMap;

        void checkFilesForChanges();

        /// @brief Returns true if saved last write time is less than the current
        bool updateWriteTimeIfChanged(ObservedAssetFile& fileOverwriteListeners);
    };
}
