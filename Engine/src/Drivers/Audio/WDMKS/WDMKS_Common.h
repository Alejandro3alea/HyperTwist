#pragma once
#include "Misc/DataTypes.h"

#ifdef _WIN32
#include <Windows.h>
#include <ks.h>
#endif

enum class DeviceSampleFormat
{
	eFloat32,
	eInt16,
	eInt24,
	eInt32,
	eInvalid
};

i32 GetBytesPerSample(DeviceSampleFormat sf);

bool WDMKS_SyncIoctl(HANDLE hHandle, ULONG octlNumber, void* pIn, ULONG inSize, void* pOut, ULONG outSize, ULONG* bytesReturned);

bool WDMKS_GetPropertySimple(HANDLE handle, const GUID* propertySet, const ULONG property,
	void* value, const ULONG valueSize, void* pInstance, const ULONG instanceSize);
bool WDMKS_SetPropertySimple(
	HANDLE handle, const GUID* propertySet, const ULONG propertyId, void* value, 
	const ULONG valueSize, void* instance, const ULONG instanceSize);

bool WDMKS_GetPinPropertySimple(HANDLE handle, const ULONG pinId, const GUID* propertySet,
	const ULONG property, void* result, const ULONG resultSize);
bool WDMKS_GetPinPropertyMulti(HANDLE handle, const ULONG pinId, const GUID* propertySet,
	const ULONG propertyId, KSMULTIPLE_ITEM** ksMultipleItem);