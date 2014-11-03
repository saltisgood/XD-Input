#ifndef __CTRLHOOK_UTIL_H_
#define __CTRLHOOK_UTIL_H_

#include <string>

namespace Hook
{
	// Print an error message to the console
	void errorMessage(const std::string& mesg);

	// Equivalent to the cmd pause command
	void pause();

	// Retrieve the first character from the console without the user having to press enter.
	// Displays the character and goes to a new line.
	char getChar();

	// Convert a wide string to a normal string
	std::string wideToByteString(const std::wstring &str);
	// Convert a normal string to a wide string
	std::wstring byteToWideString(const std::string &str);
}

#endif