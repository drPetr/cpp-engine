#pragma once
#include <memory>

namespace engine::core
{

template <typename T>
class unique_ptr : public std::unique_ptr<T>
{
private:
    typedef ::std::unique_ptr<T>    inherited;

public:
    /* inherit all constructors */
    using inherited::unique_ptr;
}; /* class unique_ptr */

} /* namespace engine::core */
