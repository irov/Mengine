#include "AppleNativePythonScriptEmbedding.h"

#include "Environment/Python/PythonScriptWrapper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {   
        //////////////////////////////////////////////////////////////////////////
        class PythonAppleSemaphoreListener
            : public AppleSemaphoreListenerInterface
        {
        public:
            PythonAppleSemaphoreListener( const pybind::object & _cb, const pybind::args & _args )
                : m_cb( _cb )
                , m_args( _args )
            {
            }

        protected:
            void invoke() override
            {
                m_cb.call_args( m_args );
            }

        protected:
            pybind::object m_cb;
            pybind::args m_args;
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<PythonAppleSemaphoreListener, AppleSemaphoreListenerInterface> PythonAppleSemaphoreListenerPtr;
        //////////////////////////////////////////////////////////////////////////
        void AppleNativePythonService_waitSemaphore( const ConstString & _name, const pybind::object & _cb, const pybind::args & _args )
        {
            AppleSemaphoreListenerInterfacePtr listener = Helper::makeFactorableUnique<PythonAppleSemaphoreListener>( MENGINE_DOCUMENT_PYBIND, _cb, _args );

            APPLE_NATIVEPYTHON_SERVICE()
                ->waitSemaphore( _name, listener );
        }
        ///////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    AppleNativePythonScriptEmbedding::AppleNativePythonScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleNativePythonScriptEmbedding::~AppleNativePythonScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleNativePythonScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        pybind::def_function_args( _kernel, "waitSemaphore", &Detail::AppleNativePythonService_waitSemaphore );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        MENGINE_UNUSED( _kernel );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
}

