#pragma once
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <memory>
#include <windows.h>

namespace engine {

typedef unsigned char   byte;
typedef unsigned short  word;
typedef unsigned int    dword;

typedef HWND    whandle_t;

template <class T>
class vector : public ::std::vector<T> {};

class string : public ::std::basic_string<char, std::char_traits<char>, std::allocator<char>> {
    typedef ::std::basic_string<char, std::char_traits<char>, std::allocator<char>>  self;
public:
    string() : self() {}
    string( const string &s ) : self(s) {}
    string( const char *cs ) : self(cs) {}
    string( const std::istreambuf_iterator<char> &begIt,
            const std::istreambuf_iterator<char> &endIt ) : self( begIt, endIt ) {}
};

template <class T>
class shared_ptr : public std::shared_ptr<T> {
    typedef ::std::shared_ptr<T>  self;
public:
    explicit shared_ptr( T *ptr ) : self( ptr ) {}
    shared_ptr( const shared_ptr &ref ) noexcept : self( ref ) {}
    shared_ptr( shared_ptr &&ref ) noexcept : self( ref ) {}
};





typedef ::std::ifstream      ifstream;
typedef ::std::ofstream      ofstream;
typedef ::std::istream      istream;
typedef ::std::ostream      ostream;
typedef ::std::iostream     iostream;

/*
class istream : public ::std::basic_istream<char> {
    typedef ::std::basic_istream<char> self;
public:
};

class ostream : public ::std::basic_ostream<char> {
    typedef ::std::basic_ostream<char> self;
public:
};
*/

struct point2d {
                point2d() {}
                point2d( int x, int y ) : x{x}, y{y} {}
    int         x, y;
};

struct size2d {
                size2d() {}
                size2d( int width, int height ) : width{width}, height{height} {}
    int         width, height;
};

struct rect2d {
    point2d     position;
    size2d      size;
};

} /* namespace engine */