#   pragma once

namespace Mengine
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