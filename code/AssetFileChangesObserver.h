#pragma once

#include <functional>
#include <filesystem>

#include "AssetContentLoader.h"
#include "Timer.h"

namespace BGLRenderer
{
    typedef std::function<void(const std::filesystem::path &, const std::filesystem::file_time_type &)> FileChangedFn;

    class AssetFileChangesObserver
    {
    public:
        AssetFileChangesObserver(const std::shared_ptr<AssetContentLoader>& contentLoader, float checkDelay = 0.5f);
        ~AssetFileChangesObserver() = default;

        void tick();

        void listenFileChanged(const std::filesystem::path& path, const FileChangedFn& callback);

    private:
        std::shared_ptr<AssetContentLoader> _contentLoader;

        HighResolutionTimer _checkTimer;
        float _checkDelay = 0.5f;

        struct ObservedAssetFile
        {
            std::filesystem::path path;
            std::filesystem::file_time_type lastWriteTime;
            std::vector<FileChangedFn> listeners;
        };
        
        std::unordered_map<std::filesystem::path, ObservedAssetFile> _observedFilesMap;

        void checkFilesForChanges();

        /// @brief Returns true if saved last write time is less than the current
        bool updateWriteTimeIfChanged(ObservedAssetFile& fileOverwriteListeners);

        void notify(const ObservedAssetFile& observedAssetFile);
    };
}
