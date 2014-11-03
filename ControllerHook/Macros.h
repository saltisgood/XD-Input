#include <forward_list>
#include <string>

#ifdef _UNICODE

	typedef std::wstring HOOK_TCHARSTR;

	#define HOOK_TCHAROUT std::wcout
	#define HOOK_TCHARIFY(x) Hook::byteToWideString(x)

#else

	typedef std::string HOOK_TCHARSTR;

	#define HOOK_TCHAROUT std::cout
	#define HOOK_TCHARIFY(x) (x)

#endif