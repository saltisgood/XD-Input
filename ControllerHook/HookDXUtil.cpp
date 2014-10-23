#include "HookDXUtil.h"

Hook::DInput8_Util *Hook::DInput8_Util::sInst = nullptr;

bool Hook::DInput8_Util::create()
{
	if (!sInst)
	{
		sInst = new DInput8_Util();

		HRESULT result = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, reinterpret_cast<LPVOID *>(&sInst->mDInput8Inst), NULL);
		
		return SUCCEEDED(result);
	}

	return sInst->mDInput8Inst != nullptr;
}

Hook::DInput8_Util::DInput8_Util() :
	mDInput8Inst(nullptr)
{}