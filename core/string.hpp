#pragma once
#include <string>
#include <core/assert.hpp>

namespace engine::core
{

class string : public std::basic_string<char, std::char_traits<char>, std::allocator<char>>
{
private:
    typedef ::std::basic_string<char, std::char_traits<char>, std::allocator<char>> inherited;

public:
    /* inherit all constructors */
    using basic_string::basic_string;

    reference       operator[]( size_type n );
    const_reference operator[]( size_type n ) const;
}; /* class string */



/* string::operator[] */
inline typename string::reference string::operator[]( size_type n )
{
    container_asserta(n < this->size(), 
        "the index '%Iu' is out of bounds of the string [0..%Iu)", n,  this->size());
    return inherited::operator[](n);
}

/* string::operator[] const */
inline typename string::const_reference string::operator[]( size_type n ) const
{
    container_asserta(n < this->size(),
        "the index '%Iu' is out of bounds of the string [0..%Iu)", n,  this->size());
    return inherited::operator[](n);
}

} /* namespace engine::core */
