#pragma once
#include "Misc/Singleton.h"
#include "Misc/ColorConsole.h"

#include <memory>
#include <iostream>
#include <string>
#include <map>

struct IResourceBase
{
	virtual ~IResourceBase() {}
};

template <typename T>
struct Resource : public IResourceBase
{
	Resource(const std::shared_ptr<T>& obj) : mResource(obj) {}

	operator T() const { return *mResource.get(); }
	T* get() const { return mResource.get(); }

private:
	std::shared_ptr<T> mResource;
};


struct IResourceImporterBase
{
	~IResourceImporterBase();

	virtual IResourceBase* Import(const std::string& path) = 0;
};

template <typename T>
struct ResourceImporter : public IResourceImporterBase
{
	virtual IResourceBase* Import(const std::string& path) override
	{
		return new Resource<T>(std::shared_ptr<T>(new T(path)));
	}

	template <class... Ts>
	IResourceBase* Import(Ts... constructorParams)
	{
		return new Resource<T>(std::shared_ptr<T>(new T(constructorParams)));
	}
};


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
