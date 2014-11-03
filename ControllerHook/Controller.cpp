#include "Controller.h"

const std::string BUTTON_STR_Y = "BUTTON_Y";
const std::string BUTTON_STR_B = "BUTTON_B";
const std::string BUTTON_STR_A = "BUTTON_A";
const std::string BUTTON_STR_X = "BUTTON_X";
const std::string BUTTON_STR_LB = "BUTTON_LB";
const std::string BUTTON_STR_RB = "BUTTON_RB";
const std::string BUTTON_STR_LT = "BUTTON_LT";
const std::string BUTTON_STR_RT = "BUTTON_RT";
const std::string BUTTON_STR_BACK = "BUTTON_BACK";
const std::string BUTTON_STR_START = "BUTTON_START";
const std::string BUTTON_STR_LS = "BUTTON_LS";
const std::string BUTTON_STR_RS = "BUTTON_RS";
const std::string BUTTON_STR_INVALID = "BUTTON_INVALID";

Hook::Buttons Hook::parseButtonString(const std::string& str)
{
#define HOOK_BUTT_STR(name) if (!str.compare(BUTTON_STR_##name)) { return Buttons::BUTTON_##name; }

	HOOK_BUTT_STR(Y)
	else HOOK_BUTT_STR(B)
	else HOOK_BUTT_STR(A)
	else HOOK_BUTT_STR(X)
	else HOOK_BUTT_STR(LB)
	else HOOK_BUTT_STR(RB)
	else HOOK_BUTT_STR(LT)
	else HOOK_BUTT_STR(RT)
	else HOOK_BUTT_STR(BACK)
	else HOOK_BUTT_STR(START)
	else HOOK_BUTT_STR(LS)
	else HOOK_BUTT_STR(RS)

	return Buttons::INVALID;

#undef HOOK_BUTT_STR
}

const std::string &Hook::getButtonString(Hook::Buttons butt)
{
#define HOOK_BUTT_STR(name) if (butt == Hook::Buttons::BUTTON_##name) { return BUTTON_STR_##name; }

	HOOK_BUTT_STR(Y)
	else HOOK_BUTT_STR(B)
	else HOOK_BUTT_STR(A)
	else HOOK_BUTT_STR(X)
	else HOOK_BUTT_STR(LB)
	else HOOK_BUTT_STR(RB)
	else HOOK_BUTT_STR(LT)
	else HOOK_BUTT_STR(RT)
	else HOOK_BUTT_STR(BACK)
	else HOOK_BUTT_STR(START)
	else HOOK_BUTT_STR(LS)
	else HOOK_BUTT_STR(RS)

	return BUTTON_STR_INVALID;
	
#undef HOOK_BUTT_STR
}