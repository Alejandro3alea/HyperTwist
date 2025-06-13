#include "WDMKS_Common.h"
#include "Misc/Requires.h"

#include <vector>
#include <string>

i32 GetBytesPerSample(DeviceSampleFormat sf)
{
	switch (sf)
	{
	case  DeviceSampleFormat::eFloat32: return 4;
	case  DeviceSampleFormat::eInt16: return 2;
	case  DeviceSampleFormat::eInt24: return 3;
	case  DeviceSampleFormat::eInt32: return 4;
	default: return -1;
	}
}

/* Low level pin/filter access functions */
bool WDMKS_SyncIoctl(HANDLE hHandle, ULONG octlNumber, void* pIn, ULONG inSize, void* pOut, ULONG outSize, ULONG* bytesReturned)
{
	ULONG dummyBytesReturned;
	if (bytesReturned == nullptr)
		bytesReturned = &dummyBytesReturned;

	OVERLAPPED overlapped;
	memset(&overlapped, 0, sizeof(overlapped));
	overlapped.hEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	Requires(!overlapped.hEvent, "CreateEvent failed with error code " + std::to_string(GetLastError()))
		overlapped.hEvent = (HANDLE)((DWORD_PTR)overlapped.hEvent | 0x1);

	int boolResult = DeviceIoControl(hHandle, octlNumber, pIn, inSize, pOut, outSize, bytesReturned, &overlapped);
	if (!boolResult)
	{
		unsigned long iError = GetLastError();
		if (iError == ERROR_IO_PENDING)
		{
			iError = WaitForSingleObject(overlapped.hEvent, INFINITE);
			if (iError != WAIT_OBJECT_0)
			{
				CloseHandle(overlapped.hEvent);
				abort();
			}
			*bytesReturned = 0;
		}
		else if ((iError == ERROR_INSUFFICIENT_BUFFER || iError == ERROR_MORE_DATA) &&
			octlNumber == IOCTL_KS_PROPERTY && outSize == 0)
		{
			boolResult = TRUE;
		}
		else
		{
			CloseHandle(overlapped.hEvent);
			abort();
		}
	}
	if (!boolResult)

		CloseHandle(overlapped.hEvent);
	return true;
}

bool WDMKS_GetPropertySimple(HANDLE handle, const GUID* propertySet, const ULONG property,
	void* value, const ULONG valueSize, void* pInstance, const ULONG instanceSize)
{
	const ULONG propertySize = sizeof(KSPROPERTY) + instanceSize;
	std::vector<char> buf;
	buf.resize(propertySize);
	KSPROPERTY* ksProperty = (KSPROPERTY*)&buf[0];

	memset(ksProperty, 0, sizeof(*ksProperty));
	ksProperty->Set = *propertySet;
	ksProperty->Id = property;
	ksProperty->Flags = KSPROPERTY_TYPE_GET;

	if (pInstance)
		memcpy(&ksProperty[1], pInstance, instanceSize);

	return WDMKS_SyncIoctl(handle, IOCTL_KS_PROPERTY, ksProperty, propertySize, value, valueSize, nullptr);
}

bool WDMKS_SetPropertySimple(HANDLE handle, const GUID* propertySet, const ULONG propertyId, void* value, 
	const ULONG valueSize, void* instance, const ULONG instanceSize)
{
	const ULONG propertySize = sizeof(KSPROPERTY) + instanceSize;
	std::vector<int8_t> buf(propertySize);
	KSPROPERTY* ksProperty = reinterpret_cast<KSPROPERTY*>(&buf[0]);

	memset(ksProperty, 0, sizeof(*ksProperty));
	ksProperty->Set = *propertySet;
	ksProperty->Id = propertyId;
	ksProperty->Flags = KSPROPERTY_TYPE_SET;

	if (instance)
		memcpy(((char*)ksProperty + sizeof(KSPROPERTY)), instance, instanceSize);

	return WDMKS_SyncIoctl(handle, IOCTL_KS_PROPERTY, ksProperty, propertySize, value, valueSize, nullptr);
}

bool WDMKS_GetPinPropertySimple(HANDLE handle, const ULONG pinId, const GUID* propertySet,
	const ULONG property, void* result, const ULONG resultSize)
{
	KSP_PIN ksPProp;
	ksPProp.Property.Set = *propertySet;
	ksPProp.Property.Id = property;
	ksPProp.Property.Flags = KSPROPERTY_TYPE_GET;
	ksPProp.PinId = pinId;
	ksPProp.Reserved = 0;

	return WDMKS_SyncIoctl(handle, IOCTL_KS_PROPERTY, &ksPProp, sizeof(KSP_PIN), result, resultSize, nullptr);
}

bool WDMKS_GetPinPropertyMulti(HANDLE handle, const ULONG pinId, const GUID* propertySet, 
	const ULONG propertyId, KSMULTIPLE_ITEM** ksMultipleItem)
{
	KSP_PIN ksPProp;

	ksPProp.Property.Set = *propertySet;
	ksPProp.Property.Id = propertyId;
	ksPProp.Property.Flags = KSPROPERTY_TYPE_GET;
	ksPProp.PinId = pinId;
	ksPProp.Reserved = 0;

	unsigned long multipleItemSize = 0;
	if (!WDMKS_SyncIoctl(handle, IOCTL_KS_PROPERTY, &ksPProp.Property, sizeof(KSP_PIN), nullptr, 0, &multipleItemSize))
		return false;

	*ksMultipleItem = reinterpret_cast<KSMULTIPLE_ITEM*>(malloc(multipleItemSize));
	Requires(*ksMultipleItem != nullptr, "[AUDIO] - Pin property retrieval failed.");

	if (!WDMKS_SyncIoctl(handle, IOCTL_KS_PROPERTY, &ksPProp, sizeof(KSP_PIN), reinterpret_cast<void*>(*ksMultipleItem), multipleItemSize, nullptr))
	{
		free(ksMultipleItem);
		return false;
	}

	return true;
}