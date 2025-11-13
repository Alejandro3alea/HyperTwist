#pragma once
#include <memory>

struct IResourceBase
{
    virtual ~IResourceBase() {}
};

template <typename T> struct Resource : public IResourceBase
{
    Resource(const std::shared_ptr<T>& obj) : mResource(obj) {}

    inline operator bool() const noexcept { return static_cast<bool>(mResource); }

    inline T* get() const noexcept { return mResource.get(); }
    inline T* operator->() const noexcept { return get(); }
    inline const T& operator*() const { return *mResource.get(); }

  private:
    std::shared_ptr<T> mResource;
};
