#ifndef __CTRLHOOK_DXUTIL_H_
#define __CTRLHOOK_DXUTIL_H_

#include <dinput.h>

namespace Hook
{
	class DInput8_Util
	{
	public:
		static bool create();

		static LPDIRECTINPUT8 retrieve();

	private:
		static DInput8_Util *sInst;

	private:
		DInput8_Util();

		LPDIRECTINPUT8 mDInput8Inst;
	};

#define HOOKDIN8_GET Hook::DInput8_Util::retrieve()

#pragma region Inlines

	inline LPDIRECTINPUT8 DInput8_Util::retrieve()
	{
		return sInst->mDInput8Inst;
	}

#pragma endregion
}

#endif