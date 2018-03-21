#pragma once

#include "stdex/stl_allocator.h"
#include <sstream>

namespace Mengine
{
	typedef std::basic_stringstream<char, std::char_traits<char>, stdex::stl_allocator<char> > Stringstream;
	typedef std::basic_stringstream<wchar_t, std::char_traits<wchar_t>, stdex::stl_allocator<wchar_t> > WStringstream;
}