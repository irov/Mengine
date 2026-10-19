#pragma once

#include "Environment/Windows/WindowsIncluder.h"

#include <wrl.h>

namespace Mengine
{
    template<class T>
    class Win32RuntimeClassWeak
        : public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>, IUnknown>
    {
    public:
        explicit Win32RuntimeClassWeak( T * _value )
            : m_value( _value )
        {
        }

        ~Win32RuntimeClassWeak() override
        {
        }

    public:
        T * get() const
        {
            return m_value;
        }

    public:
        void reset()
        {
            m_value = nullptr;
        }

    private:
        T * m_value;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    using Win32RuntimeClassWeakPtr = Microsoft::WRL::ComPtr<Win32RuntimeClassWeak<T>>;
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        Win32RuntimeClassWeakPtr<T> makeWin32RuntimeClassWeak( T * _value )
        {
            Win32RuntimeClassWeakPtr<T> weak = Microsoft::WRL::Make<Win32RuntimeClassWeak<T>>( _value );

            return weak;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
