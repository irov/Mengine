#   pragma once

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    class ServantBase
        : public T
    {
    public:
        ServantBase()
        {
        }

        virtual ~ServantBase()
        {
        }
    };
}