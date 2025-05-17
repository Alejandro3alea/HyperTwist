#pragma once
#include "Drivers/DriverBase.h"

#ifdef _WIN32
#include <Windows.h>
#include <ks.h>
#endif



class WDMKS_AudioDriver : public DriverBase
{
	virtual void Initialize() override;

private:

protected:
#ifdef _WIN32
	HMODULE mKernelStreamingUserDll = nullptr;

	using KsCreatePinFunc = KSDDKAPI DWORD(WINAPI*) (HANDLE, PKSPIN_CONNECT, ACCESS_MASK, PHANDLE);
	KsCreatePinFunc* CreatePin = nullptr;
#endif
};