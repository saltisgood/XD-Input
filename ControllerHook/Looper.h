#ifndef __CTRLHOOK_LOOPER_H_
#define __CTRLHOOK_LOOPER_H_

#include <guiddef.h>

namespace Hook
{
	class Looper
	{
	public:
		Looper(GUID activeGUID);

		bool loop();

	private:
		GUID mGuid;
	};
}

#endif