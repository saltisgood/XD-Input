#include "Util.h"

#include <iostream>
#include <conio.h>
#include <codecvt>

void Hook::errorMessage(const std::string& msg)
{
	std::cout << "Error!!! " << msg << std::endl;
}

void Hook::pause()
{
	std::cout << "Press any key to continue..." << std::endl;
	_getch();
}

char Hook::getChar()
{
	auto c(_getch());
	printf_s("%c\n", c);
	return c;
}

std::string Hook::wideToByteString(const std::wstring &str)
{
	return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(str);
}

std::wstring Hook::byteToWideString(const std::string &str)
{
	return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().from_bytes(str);
}