#ifndef __CTRLHOOK_UTIL_H_
#define __CTRLHOOK_UTIL_H_

#include <string>

namespace Hook
{
	void errorMessage(const std::string& mesg);

	void pause();

	char getChar();

	std::string wideToByteString(const std::wstring &str);
	std::wstring byteToWideString(const std::string &str);
}

#endif