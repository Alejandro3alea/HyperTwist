#pragma once
#include "ResourceImporter.h"
#include "Misc/Singleton.h"
#include "Misc/ColorConsole.h"
#include "Utils/FileUtils.h"

#include <iostream>
#include <map>

class ResourceManager
{
	Singleton(ResourceManager);

public:
	void Initialize();
	void LoadDefaultAssets();
	void Shutdown();

	template <typename T, typename... Args>
	Resource<T>* Load(const std::string& path, Args&&... args);

	template <typename T, typename... Args>
	Resource<T>* LoadFromGlobalPath(const std::string& fullPath, Args&&... args);

	template <typename T>
	Resource<T>* GetDefaultAsset();

	const std::string GetExtension(const std::string& filePath) const;
	const std::string GetResourceName(const std::string& filePath) const;
	const std::string GetPathWithoutExtension(const std::string& filePath) const;

	void ReloadShaders();

public:
	std::map<std::string, std::shared_ptr<IResourceImporterBase>> mAllImporters;
	std::map<std::string, std::shared_ptr<IResourceBase>> mAllResources;
	std::map<std::string, IResourceBase*> mDefaultResources;

private:
	std::string mBasePath;
};

#define ResourceMgr ResourceManager::Instance()


template<typename T, typename... Args>
inline Resource<T>* ResourceManager::Load(const std::string& path, Args&&... args)
{
	const std::string ext = GetExtension(path);
	const std::string fullPath = FileUtils::JoinPath(DATA_PATH, path);

	if (mAllImporters.find(ext) == mAllImporters.end())
	{
		PrintWarning("There is no proper extension for \"." + ext + "\" files yet.");
		return nullptr;
	}

	if (mAllResources.find(fullPath) != mAllResources.end())
		return  dynamic_cast<Resource<T>*>(mAllResources[fullPath].get());
	
	try
	{
		ResourceImporter<T>* importer = dynamic_cast<ResourceImporter<T>*>(mAllImporters[ext].get());
		Resource<T>* resource = dynamic_cast<Resource<T>*>(importer->Import(fullPath, std::forward<Args>(args)...));
		mAllResources[fullPath] = std::shared_ptr<IResourceBase>(resource);
		return resource;
	}
	catch (const ResourceLoadException& exc)
	{
		PrintWarning(exc.reason);
		Resource<T>* resource = new Resource<T>(*GetDefaultAsset<T>());
		mAllResources[fullPath] = std::shared_ptr<IResourceBase>(resource);
		return resource;
	}
}

template<typename T, typename... Args>
inline Resource<T>* ResourceManager::LoadFromGlobalPath(const std::string& fullPath, Args&&... args)
{
	const std::string ext = GetExtension(fullPath);

	if (mAllImporters.find(ext) == mAllImporters.end())
	{
		PrintWarning("There is no proper extension for \"." + ext + "\" files yet.");
		return nullptr;
	}

	if (mAllResources.find(fullPath) != mAllResources.end())
		return  dynamic_cast<Resource<T>*>(mAllResources[fullPath].get());

	try
	{
		ResourceImporter<T>* importer = dynamic_cast<ResourceImporter<T>*>(mAllImporters[ext].get());
		Resource<T>* resource = dynamic_cast<Resource<T>*>(importer->Import(fullPath, std::forward<Args>(args)...));
		mAllResources[fullPath] = std::shared_ptr<IResourceBase>(resource);
		return resource;
	}
	catch (const ResourceLoadException& exc)
	{
		PrintWarning(exc.reason);
		Resource<T>* resource = new Resource<T>(*GetDefaultAsset<T>());
		mAllResources[fullPath] = std::shared_ptr<IResourceBase>(resource);
		return resource;
	}
}

template<typename T>
inline Resource<T>* ResourceManager::GetDefaultAsset()
{
	const std::string assetName = typeid(T).name();
	if (mDefaultResources.find(assetName) == mDefaultResources.end())
	{
		PrintWarning("There is no proper default asset for \"." + assetName + "\" yet.");
		return nullptr;
	}

	return dynamic_cast<Resource<T>*>(mDefaultResources[assetName]);
}
