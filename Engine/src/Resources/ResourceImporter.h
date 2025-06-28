#pragma once
#include "ResourceMgr.h"
#include "Resource.h"
#include "ResourceLoadException.h"

#include <string>

struct IResourceImporterBase
{
	virtual ~IResourceImporterBase();

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
		return new Resource<T>(std::make_shared<T>(constructorParams...));
	}
};