#pragma once
#include "ResourceImporter.h"
#include "Misc/Singleton.h"
#include "Misc/ColorConsole.h"

#include <iostream>
#include <map>

class ResourceManager
{
	Singleton(ResourceManager);

public:
	void Initialize();
	void Shutdown();

	template <typename T>
	Resource<T>* Load(const std::string& path);

	template <typename T>
	Resource<T>* LoadFromBasePath(const std::string& path);

	const std::string GetExtension(const std::string& filePath) const;
	const std::string GetResourceName(const std::string& filePath) const;
	const std::string GetPathWithoutExtension(const std::string& filePath) const;

	void ReloadShaders();

public:
	std::map<std::string, std::shared_ptr<IResourceImporterBase>> mAllImporters;
	std::map<std::string, std::shared_ptr<IResourceBase>> mAllResources;

private:
	std::string mBasePath;
};

#define ResourceMgr ResourceManager::Instance()

template<typename T>
inline Resource<T>* ResourceManager::LoadFromBasePath(const std::string& path)
{
	return Load<T>(mBasePath + '\\' + path);
}

template<typename T>
inline Resource<T>* ResourceManager::Load(const std::string& path)
{
	const std::string ext = GetExtension(path);

	if (mAllImporters.find(ext) == mAllImporters.end())
	{
		PrintWarning("There is no proper extension for \"." + ext + "\" files yet.");
		return nullptr;
	}

	if (mAllResources.find(path) != mAllResources.end())
		return  dynamic_cast<Resource<T>*>(mAllResources[path].get());

	Resource<T>* resource = dynamic_cast<Resource<T>*>(mAllImporters[ext].get()->Import(path));
	mAllResources[path] = std::shared_ptr<IResourceBase>(resource);

	return resource;
}
