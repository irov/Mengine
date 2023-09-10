#pragma once

#include "Interface/ValidatorInterface.h"

namespace Mengine
{
    template<class T>
    class Validator
        : public ValidatorInterface
    {
    public:
        bool validate( const FactorablePtr & _factorable ) const override
        {
            T t = T::from( _factorable );

            return this->_validate( t );
        }

    protected:
        virtual bool _validate( const T & _t ) const = 0;
    };
}