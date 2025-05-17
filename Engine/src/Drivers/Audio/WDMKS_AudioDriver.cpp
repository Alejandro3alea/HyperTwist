#include "WDMKS_AudioDriver.h"
#include "Misc/Requires.h"

#include <vector>

void WDMKS_AudioDriver::Initialize()
{
	mKernelStreamingUserDll = LoadLibrary("ksuser.dll");
	Requires(mKernelStreamingUserDll != nullptr, "[AUDIO] - Kernel Streaming dll failed loading.");

	CreatePin = (KsCreatePinFunc*) GetProcAddress(mKernelStreamingUserDll, "KsCreatePin");
	Requires(CreatePin != nullptr, "[AUDIO] - KsCreatePin function retrieval error.");

	std::vector<WDMKS_Filter*> apFilters;
	if (!BuildFilterList(apFilters, sError))
		return "Error building filter list: " + sError;
	if (apFilters.empty())
		return "No supported audio devices found";

}
