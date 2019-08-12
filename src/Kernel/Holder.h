#pragma once

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    class Holder
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

    protected:
        static T * m_hostage;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    T * Holder<T>::m_hostage = nullptr;
    //////////////////////////////////////////////////////////////////////////
}