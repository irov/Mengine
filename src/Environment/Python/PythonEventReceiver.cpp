#include "PythonEventReceiver.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PythonEventReceiver::PythonEventReceiver()
        : m_kernel( nullptr )
    {
    };
    //////////////////////////////////////////////////////////////////////////
    PythonEventReceiver::~PythonEventReceiver()
    {
    };
    //////////////////////////////////////////////////////////////////////////    
    void PythonEventReceiver::initialize( pybind::kernel_interface * _kernel, const ConstString & _method, const pybind::object & _cb )
    {
        m_kernel = _kernel;
        m_method = _method;
        m_cb = _cb;
    }
    //////////////////////////////////////////////////////////////////////////
    pybind::kernel_interface * PythonEventReceiver::getKernel() const
    {
        return m_kernel;
    }
    //////////////////////////////////////////////////////////////////////////    
    const ConstString & PythonEventReceiver::getMethod() const
    {
        return m_method;
    }
    //////////////////////////////////////////////////////////////////////////    
    const pybind::object & PythonEventReceiver::getCb() const
    {
        return m_cb;
    }
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void assertPythonEventReceiver( const Factorable * _factorable, const pybind::dict & _kwds )
        {
            if( _kwds.empty() == true )
            {
                return;
            }

            for( pybind::dict::iterator
                it = _kwds.begin(),
                it_end = _kwds.end();
                it != it_end;
                ++it )
            {
                String k = it.key();

                LOGGER_ERROR( "node type '%s' invalid kwds '%s'"
                    , _factorable->getType().c_str()
                    , k.c_str()
                );
            }

            MENGINE_ERROR_FATAL( "invalid python event receiver type '%s'"
                , _factorable->getType().c_str()
            );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_DEBUG)
#   define MENGINE_ASSERTION_PYTHON_EVENT_RECEIVER(Factorable, Kwds)\
    Helper::assertPythonEventReceiver(Factorable, Kwds)
#else
#   define MENGINE_ASSERTION_PYTHON_EVENT_RECEIVER(Identity, Kwds)
#endif
//////////////////////////////////////////////////////////////////////////
