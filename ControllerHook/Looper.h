#ifndef __CTRLHOOK_LOOPER_H_
#define __CTRLHOOK_LOOPER_H_

#include <guiddef.h>

#include "ScpDevice.h"

namespace Hook
{
	class Looper
	{
	public:
		Looper(GUID activeGUID);

		bool loop(Scp::VirtualDevice& vjd);

	private:
		GUID mGuid;
	};
}

#endif