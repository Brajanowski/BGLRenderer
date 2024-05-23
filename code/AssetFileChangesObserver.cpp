#include "AssetFileChangesObserver.h"

namespace BGLRenderer
{
    AssetFileChangesObserver::AssetFileChangesObserver(const std::shared_ptr<AssetContentLoader>& contentLoader, float checkDelay) :
        _contentLoader(contentLoader),
        _checkDelay(checkDelay)
    {
        _checkTimer.restart();
    }

    void AssetFileChangesObserver::tick()
    {
        if (_checkTimer.elapsedSeconds() >= _checkDelay)
        {
            _checkTimer.restart();
            checkFilesForChanges();
        }
    }
    
    void AssetFileChangesObserver::listenFileChanged(const std::filesystem::path& path, const FileChangedFn& callback)
    {
        ASSERT(callback != nullptr, "Trying to add null callback!");
        ASSERT(_contentLoader->fileExists(path), "File doesn't exists!");

        if (_observedFilesMap.contains(path))
        {
            auto& fileListeners = _observedFilesMap[path];
            fileListeners.listeners.push_back(callback);
            return;
        }

        ObservedAssetFile observedAssetFile{};
        observedAssetFile.path = path;
        observedAssetFile.lastWriteTime = _contentLoader->getLastWriteTime(path);
        observedAssetFile.listeners.push_back(callback);
        _observedFilesMap[path] = observedAssetFile;
    }

    void AssetFileChangesObserver::checkFilesForChanges()
    {
        for (auto& [key, val] : _observedFilesMap)
        {
            if (updateWriteTimeIfChanged(val))
            {
                notify(val);
            }
        }
    }

    bool AssetFileChangesObserver::updateWriteTimeIfChanged(ObservedAssetFile& fileOverwriteListeners)
    {
        std::filesystem::file_time_type currentWriteTime = _contentLoader->getLastWriteTime(fileOverwriteListeners.path);

        if (currentWriteTime <= fileOverwriteListeners.lastWriteTime)
        {
            return false;
        }

        fileOverwriteListeners.lastWriteTime = currentWriteTime;
        return true;
    }

    void AssetFileChangesObserver::notify(const ObservedAssetFile& observedAssetFile)
    {
        for (const auto& listener : observedAssetFile.listeners)
        {
            listener(observedAssetFile.path, observedAssetFile.lastWriteTime);
        }
    }
}
