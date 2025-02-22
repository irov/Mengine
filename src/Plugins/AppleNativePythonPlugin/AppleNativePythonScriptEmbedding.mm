#include "AppleNativePythonScriptEmbedding.h"

#import "Environment/Apple/AppleSemaphoreListenerInterface.h"
#import "Environment/Python/PythonScriptWrapper.h"
#import "Environment/Python/PythonCallbackProvider.h"

#include "AppleNativePythonInterface.h"

#include "Kernel/ThreadHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {   
        //////////////////////////////////////////////////////////////////////////
        class PythonAppleSemaphoreListener
            : public AppleSemaphoreListenerInterface
            , public PythonCallbackProvider
        {
            DECLARE_FACTORABLE("PythonAppleSemaphoreListener")
            
        public:
            PythonAppleSemaphoreListener( const pybind::object & _cb, const pybind::args & _args )
                : PythonCallbackProvider( _cb, _args )
            {
            }

        protected:
            void invoke() override
            {
                PythonCallbackProviderPtr keep = PythonCallbackProviderPtr::from(this);

                Helper::dispatchMainThreadEvent([keep]() {
                    keep->call_cb();
                });
            }
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
    void AppleNativePythonScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        MENGINE_UNUSED( _kernel );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
}

