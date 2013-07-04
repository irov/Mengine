#   pragma once

#   include "Core/ConstStringHolder.h"

#   include "Factory/Factory.h"

namespace Menge
{    
    //////////////////////////////////////////////////////////////////////////
    class ConstStringHolderNone
        : public ConstStringHolder
        , public Factory
    {
    public:
        ConstStringHolderNone()
        {
            const char * empty_str = "";

            this->setup( empty_str, 0 );

            this->setFactory( this );
        }

    protected:
        void _releaseString() override
        {
            //Empty
        }

    protected:
        Factorable * _createObject() override
        {
            return nullptr;
        }

    protected:
        void _destroyObject( Factorable * _object ) override
        {
            (void)_object;
        }
    };
}