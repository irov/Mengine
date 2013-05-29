#   pragma once

#   include "Core/ConstStringHolder.h"

#   include "Factory/Factory.h"

namespace Menge
{    
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        template<class T>
        const T * cs_empty_str();
    }
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    class ConstStringHolderNone
        : public ConstStringHolderT<T>
        , public Factory
    {
    public:
        ConstStringHolderNone()
            : ConstStringHolderT<T>()
        {
            const T * empty_str = Detail::cs_empty_str<T>();
            this->setup(empty_str, 0);

            this->setFactory( this );
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