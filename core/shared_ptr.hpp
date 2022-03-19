#pragma once
#include <memory>

namespace engine::core
{

template <typename T>
class shared_ptr : public std::shared_ptr<T>
{
private:
    typedef ::std::shared_ptr<T>    inherited;

public:
    /* inherit all constructors */
    using inherited::shared_ptr;
}; /* class shared_ptr */

} /* namespace engine::core */
