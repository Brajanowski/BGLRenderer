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
    
    Publisher<AssetFileChangedEvent>::ListenerHandle AssetFileChangesObserver::listenFileChanged(const std::filesystem::path& path, const Publisher<AssetFileChangedEvent>::CallbackFn& callback)
    {
        ASSERT(callback != nullptr, "Trying to add null callback!");
        ASSERT(_contentLoader->fileExists(path), "File doesn't exists!");

        if (_observedFilesMap.contains(path))
        {
            auto& fileListeners = _observedFilesMap[path];
            return fileListeners.publisher.listen(callback);
        }

        ObservedAssetFile observedAssetFile{};
        observedAssetFile.path = path;
        observedAssetFile.lastWriteTime = _contentLoader->getLastWriteTime(path);
        Publisher<AssetFileChangedEvent>::ListenerHandle listenerHandle = observedAssetFile.publisher.listen(callback);
        _observedFilesMap[path] = observedAssetFile;

        return listenerHandle;
    }

    void AssetFileChangesObserver::removeFileChangedListener(const std::filesystem::path& path, Publisher<AssetFileChangedEvent>::ListenerHandle handle)
    {
        if (!_observedFilesMap.contains(path))
        {
            return;
        }

        ObservedAssetFile& observedFile = _observedFilesMap[path];
        observedFile.publisher.removeListener(handle);

        if (observedFile.publisher.listenersCount() == 0)
        {
            _observedFilesMap.erase(path);
        }
    }

    void AssetFileChangesObserver::checkFilesForChanges()
    {
        for (auto& [key, val] : _observedFilesMap)
        {
            if (updateWriteTimeIfChanged(val))
            {
                val.publisher.publish({val.path, val.lastWriteTime});
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
}
