#pragma once

#include "ResourceValidatorInterface.h"

namespace Mengine
{    
    template<class T>
    class ResourceValidatorT
        : public ResourceValidatorInterface
    {
    public:
        bool validate( const ResourcePtr & _resource ) override
        {
            const T & resourceT = stdex::intrusive_static_cast<T>(_resource);

            return this->_validate( resourceT );
        }

    protected:
        virtual bool _validate( const T & _resource ) = 0;
    };
}