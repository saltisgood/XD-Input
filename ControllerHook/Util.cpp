#include "Util.h"

#include <iostream>
#include <conio.h>

void Hook::errorMessage(const std::string& msg)
{
	std::cout << "Error!!! " << msg << std::endl;
}

void Hook::pause()
{
	std::cout << "Press any key to continue..." << std::endl;
	_getch();
}