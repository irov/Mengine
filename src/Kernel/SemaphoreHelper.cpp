#include "SemaphoreHelper.h"

#include "Interface/SemaphoreServiceInterface.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/DocumentHelper.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void activateSemaphore( const ConstString & _name )
        {
            SEMAPHORE_SERVICE()
                ->activateSemaphore( _name );
        }
        //////////////////////////////////////////////////////////////////////////
        void deactivateSemaphore( const ConstString & _name )
        {
            SEMAPHORE_SERVICE()
                ->deactivateSemaphore( _name );
        }
        //////////////////////////////////////////////////////////////////////////
        namespace
        {
            class LambdaSemaphoreListener
                : public SemaphoreListenerInterface
                , public Factorable
            {
            public:
                LambdaSemaphoreListener( const LambdaSemaphore & _lambda )
                    : m_lambda( _lambda )
                {
                }

                ~LambdaSemaphoreListener() override
                {
                }

            protected:
                void onActivated() override
                {
                    m_lambda();
                }

            protected:
                LambdaSemaphore m_lambda;
            };
        }
        //////////////////////////////////////////////////////////////////////////
        void waitSemaphore( const ConstString & _name, const LambdaSemaphore & _semaphore )
        {
            SemaphoreListenerInterfacePtr listener = Helper::makeFactorableUnique<LambdaSemaphoreListener>( MENGINE_DOCUMENT_FUNCTION, _semaphore );

            SEMAPHORE_SERVICE()
                ->waitSemaphore( _name, listener );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}