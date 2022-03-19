#pragma once
#include <fstream>
#include <windows.h>

namespace engine
{

typedef unsigned char   byte;
typedef unsigned short  word;
typedef unsigned int    dword;

typedef HWND            whandle_t;

typedef ::std::ifstream     ifstream;
typedef ::std::ofstream     ofstream;
typedef ::std::istream      istream;
typedef ::std::ostream      ostream;
typedef ::std::iostream     iostream;

} /* namespace engine::core */
