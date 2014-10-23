#include "Util.h"

#include <iostream>

void Hook::errorMessage(const std::string& msg)
{
	std::cout << "Error!!! " << msg << std::endl;
}