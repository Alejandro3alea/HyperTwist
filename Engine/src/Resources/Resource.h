#pragma once
#include <memory>
	
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
