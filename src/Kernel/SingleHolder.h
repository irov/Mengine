#pragma once

#include "Config/Config.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    class SingleHolder
    {
    public:
        static void keep( T * _hostage )
        {
            m_hostage = _hostage;
        }

        static T * get()
        {
            return m_hostage;
        }

        static T * put()
        {
            T * hostage = m_hostage;
            m_hostage = nullptr;

            return hostage;
        }

        static bool isKeep()
        {
            if( m_hostage == nullptr )
            {
                return false;
            }

            return true;
        }

    protected:
        static T * m_hostage;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    T * SingleHolder<T>::m_hostage = nullptr;
    //////////////////////////////////////////////////////////////////////////
}