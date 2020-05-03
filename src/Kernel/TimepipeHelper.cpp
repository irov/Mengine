#include "TimepipeHelper.h"

#include "Interface/TimepipeServiceInterface.h"

#include "Kernel/FactorableUnique.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        namespace Detail
        {
            class MyTimepipe
                : public TimepipeInterface
                , public Factorable
            {
            public:
                MyTimepipe( const LambdaTimepipe & _lambda )
                    : m_lambda( _lambda )
                {
                }

                ~MyTimepipe() override
                {
                }

            protected:
                void onTimepipe( const UpdateContext * _contet ) override
                {
                    m_lambda( _contet );
                }

            protected:
                LambdaTimepipe m_lambda;
            };
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t addTimepipe( const LambdaTimepipe & _lambda, const DocumentPtr & _doc )
        {
            TimepipeInterfacePtr timepipe = Helper::makeFactorableUnique<Detail::MyTimepipe>( _doc, _lambda );

            uint32_t id = TIMEPIPE_SERVICE()
                ->addTimepipe( timepipe, _doc );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        void removeTimepipe( uint32_t _id )
        {
            TIMEPIPE_SERVICE()
                ->removeTimepipe( _id );
        }
    }
}