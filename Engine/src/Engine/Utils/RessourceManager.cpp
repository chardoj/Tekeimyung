/**
* @Author   Guillaume Labey
*/

#include <Engine/Utils/Logger.hpp>
#include <dirent.h>
#include <fstream>
#include <algorithm>
#include <vector>

#include <Engine/Graphics/Geometries/Geometry.hpp>
#include <Engine/Graphics/Material.hpp>
#include <Engine/Graphics/Model.hpp>
#include <Engine/Graphics/Texture.hpp>
#include <Engine/Utils/Exception.hpp>
#include <Engine/Utils/File.hpp>
#include <Engine/Utils/Helper.hpp>
#include <Engine/Sound/SoundManager.hpp>

#include <Engine/Utils/RessourceManager.hpp>

RessourceManager*   RessourceManager::_ressourceManager = nullptr;

RessourceManager::RessourceManager() {}

RessourceManager::~RessourceManager() {}

void    RessourceManager::loadResources(const std::string& directory)
{
    DIR* dir;
    struct dirent* ent;
    RessourceManager* ressourceManager = RessourceManager::getInstance();
    std::vector<std::string> texturesExtensions = { TEXTURES_EXT };
    std::vector<std::string> modelsExtensions = { MODELS_EXT };
    std::vector<std::string> materialsExtensions = { MATERIALS_EXT };
    std::vector<std::string> soundsExtensions = { SOUNDS_EXT };

    dir = opendir(directory.c_str());
    if (!dir)
        EXCEPT(FileNotFoundException, "Cannot open resource directory \"%s\"", directory.c_str());

    while ((ent = readdir(dir)) != NULL)
    {
        // No file extension, is directory
        if (std::string(ent->d_name).find(".") == std::string::npos)
        {
            // Load directory
            std::string directoryPath = std::string(directory).append("/").append(ent->d_name);
            loadResources(directoryPath);
        }
        else
        {
            std::string file = std::string(directory).append("/").append(ent->d_name);
            std::string extension = RessourceManager::getFileExtension(ent->d_name);
            std::string basename = getBasename(file);

            extension = Helper::lowerCaseString(extension);
            // Texture resource
            if (std::find(texturesExtensions.cbegin(), texturesExtensions.cend(), extension) != texturesExtensions.cend())
            {
                // Textures could be loaded from model (It could conflict)
                getOrLoadResource<Texture>(file);
            }
            // Model resource
            else if (std::find(modelsExtensions.cbegin(), modelsExtensions.cend(), extension) != modelsExtensions.cend())
            {
                loadResource<Model>(file);
            }
            // Materials resource
            else if (std::find(materialsExtensions.cbegin(), materialsExtensions.cend(), extension) != materialsExtensions.cend())
            {
                loadResource<Material>(file);
            }
            else if (std::find(soundsExtensions.cbegin(), soundsExtensions.cend(), extension) != soundsExtensions.cend())
            {
                loadSound(basename, file);
            }
        }
    }

    closedir(dir);
}

RessourceManager*   RessourceManager::getInstance()
{
    if (!_ressourceManager)
    {
        _ressourceManager = new RessourceManager();
    }

    return (_ressourceManager);
}

std::string  RessourceManager::getFileExtension(const std::string& fileName)
{
    std::size_t size = fileName.rfind('.', fileName.length());
    if (size != std::string::npos)
        return (fileName.substr(size + 1, fileName.length() - size));

    return ("");
}

std::string RessourceManager::getBasename(const std::string& fileName)
{
    size_t basenameOcur;
    std::string basename = fileName;

    basenameOcur = fileName.find_last_of('/');
    if (basenameOcur != std::string::npos)
    {
        basename = fileName.substr(basenameOcur + 1);
    }

    return (basename);
}

const std::vector<RessourceManager::sSoundStrings>&  RessourceManager::getSoundsStrings() const
{
    return (_soundsStrings);
}

void    RessourceManager::loadSound(const std::string basename, const std::string& fileName)
{
    // Add sound string to Ressource Manager
    sSoundStrings soundString;
    soundString.path = fileName;
    soundString.name = basename;
    _soundsStrings.push_back(soundString);

    // Load sound in Sound Manager
    SoundManager::getInstance()->registerSound(fileName);
}

template<typename T>
T*  RessourceManager::getResource(const std::string& name)
{
    // Resources are stored with their basename
    std::string basename = getBasename(name);

    // The resource is not loaded
    auto& resourceMap = _resources[T::getResourceType()];
    if (resourceMap.find(basename) == resourceMap.end())
    {
        return (nullptr);
    }

    auto& resource = resourceMap.at(basename);
    return (static_cast<T*>(resource.get()));
}

template<typename T>
T*  RessourceManager::getOrLoadResource(const std::string& name)
{
    T* resource = getResource<T>(name);

    // The resource is not loaded
    if (!resource)
    {
        return loadResource<T>(name);
    }

    return (resource);
}

template<typename T>
T*  RessourceManager::loadResource(const std::string& name)
{
    std::string basename = getBasename(name);
    std::unique_ptr<T> resource = std::make_unique<T>();

    if (!resource->loadFromFile(name))
    {
        return (nullptr);
    }

    auto& resourceMap = _resources[T::getResourceType()];

    resource->setId(basename);
    resource->setPath(name);
    resourceMap[basename] = std::move(resource);
    // TODO: Allocate char* for resource name
    _resourcesNames[T::getResourceType()].push_back(resourceMap.find(basename)->first.c_str());

    return (static_cast<T*>(resourceMap[basename].get()));
}

template<typename T>
T*  RessourceManager::registerResource(std::unique_ptr<T> resource, const std::string& path)
{
    std::string basename = getBasename(path);
    auto& resourceMap = _resources[T::getResourceType()];

    if (resourceMap[basename] != nullptr)
    {
        LOG_WARN("RessourceManager::registerResource: registering resource \"%s\" that does already exists", basename.c_str());
    }

    resource->setId(basename);
    resource->setPath(path);
    resourceMap[basename] = std::move(resource);
    // TODO: Allocate char* for resource name
    _resourcesNames[T::getResourceType()].push_back(resourceMap.find(basename)->first.c_str());

    return (static_cast<T*>(resourceMap[basename].get()));
}

template<typename T>
T* RessourceManager::getOrCreateResource(const std::string& path)
{
    T* resource = getResource<T>(path);

    // The resource is not loaded
    if (!resource)
    {
        return registerResource<T>(std::make_unique<T>(), path);
    }

    return (resource);
}

template<typename T>
std::vector<const char*>& RessourceManager::getResourcesNames()
{
    return _resourcesNames[T::getResourceType()];
}

template Model*  RessourceManager::getOrLoadResource<Model>(const std::string& fileName);
template Texture*  RessourceManager::getOrLoadResource<Texture>(const std::string& fileName);
template Geometry*  RessourceManager::getOrLoadResource<Geometry>(const std::string& fileName);
template Material*  RessourceManager::getOrLoadResource<Material>(const std::string& fileName);
template File*  RessourceManager::getOrLoadResource<File>(const std::string& fileName);

template Model*  RessourceManager::getResource<Model>(const std::string& fileName);
template Texture*  RessourceManager::getResource<Texture>(const std::string& fileName);
template Geometry*  RessourceManager::getResource<Geometry>(const std::string& fileName);
template Material*  RessourceManager::getResource<Material>(const std::string& fileName);
template File*  RessourceManager::getResource<File>(const std::string& fileName);

template Model*  RessourceManager::registerResource<Model>(std::unique_ptr<Model> resource, const std::string& path);
template Texture*  RessourceManager::registerResource<Texture>(std::unique_ptr<Texture> resource, const std::string& path);
template Geometry*  RessourceManager::registerResource<Geometry>(std::unique_ptr<Geometry> resource, const std::string& path);
template Material*  RessourceManager::registerResource<Material>(std::unique_ptr<Material> resource, const std::string& path);
template File*  RessourceManager::registerResource<File>(std::unique_ptr<File> resource, const std::string& path);

template File*  RessourceManager::getOrCreateResource<File>(const std::string& path);

template std::vector<const char*>&  RessourceManager::getResourcesNames<Model>();
template std::vector<const char*>&  RessourceManager::getResourcesNames<Texture>();
template std::vector<const char*>&  RessourceManager::getResourcesNames<Material>();
