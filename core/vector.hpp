#pragma once
#include <vector>
#include <core/assert.hpp>

namespace engine::core
{

template <typename T, typename Alloc = std::allocator<T>>
class vector : public std::vector <T, Alloc>
{
private:
    typedef ::std::vector<T, Alloc>             inherited;

public:
    typedef typename inherited::size_type       size_type;
    typedef typename inherited::reference       reference;
    typedef typename inherited::const_reference const_reference;

public:
    /* inherit all constructors */
    using inherited::vector;

    reference       operator[]( size_type n );
    const_reference operator[]( size_type n ) const;
}; /* class vector */



/* vector::operator[] */
template <typename T, typename Alloc>
inline typename vector<T, Alloc>::reference vector<T, Alloc>::operator[]( size_type n )
{
    container_asserta((n >= 0) && (n < this->size()), 
        "the index '%Iu' is out of bounds of the vector [0..%Iu)", n,  this->size());
    return inherited::operator[](n);
}

/* vector::operator[] const */
template <typename T, typename Alloc>
inline typename vector <T, Alloc>::const_reference vector <T, Alloc>::operator[]( size_type n ) const
{
    container_asserta((n >= 0) && (n < this->size()), 
        "the index '%Iu' is out of bounds of the vector [0..%Iu)", n,  this->size());
    return inherited::operator[](n);
}

} /* namespace engine::core */
