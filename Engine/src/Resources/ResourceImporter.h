#pragma once
#include "Resource.h"

#include <string>

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