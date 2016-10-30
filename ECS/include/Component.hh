#pragma once

#include <typeinfo>

struct sComponent
{
    virtual const std::type_info& getTypeInfo()
    {
        return (typeid(*this));
    }

    virtual sComponent* clone() = 0;
    virtual void        update(sComponent* component) = 0;
};
