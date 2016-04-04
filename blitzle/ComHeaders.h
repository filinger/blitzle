#pragma once

#include <string>
#include <iostream>

#include <comdef.h>

#define CHECK_HR(hr) checkHr(hr, __FILE__, __LINE__)

#define CHECKED(hr, function) { hr = function; CHECK_HR(hr); }

inline void checkHr(HRESULT hr, char const* file, int line) 
{
	if (!SUCCEEDED(hr))
	{
		_com_error error(hr);
		std::wcout << file << ":" << line << ":" << hr << ": " << std::wstring(error.ErrorMessage()) << std::endl;
		exit(-1);
	}
}
