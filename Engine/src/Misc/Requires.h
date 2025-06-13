#pragma once
#include "Misc/ColorConsole.h"

#undef Requires
#define Requires(condition, message)		\
	{										\
		bool _conditionVal = condition;		\
		if (_conditionVal == false)			\
		{									\
			PrintError(message);			\
			abort();						\
		}									\
	}
	