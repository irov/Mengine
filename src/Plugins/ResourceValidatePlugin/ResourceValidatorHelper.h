#pragma once

#include "ResourceValidatorInterface.h"

namespace Mengine
{
    template<class T>
    class ResourceValidatorT
        : public ResourceValidatorInterface
    {
    public:
        bool validate( const ResourcePtr & _resource ) const override
        {
            T resourceT = T::from( _resource );

            return this->_validate( resourceT );
        }

    protected:
        virtual bool _validate( const T & _resource ) const = 0;
    };
}