#include "TimepipeHelper.h"

#include "Interface/TimepipeServiceInterface.h"

#include "Kernel/FactorableUnique.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void addTimepipe( const TimepipeInterfacePtr & _timepipe, const DocumentInterfacePtr & _doc )
        {
            TIMEPIPE_SERVICE()
                ->addTimepipe( _timepipe, _doc );
        }
        //////////////////////////////////////////////////////////////////////////
        namespace Detail
        {
            class MyTimepipe
                : public Factorable
                , public TimepipeInterface
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
        TimepipeInterfacePtr addTimepipe( const LambdaTimepipe & _lambda, const DocumentInterfacePtr & _doc )
        {
            TimepipeInterfacePtr timepipe = Helper::makeFactorableUnique<Detail::MyTimepipe>( _doc, _lambda );

            TIMEPIPE_SERVICE()
                ->addTimepipe( timepipe, _doc );

            return timepipe;
        }
        //////////////////////////////////////////////////////////////////////////
        void removeTimepipe( const TimepipeInterfacePtr & _timepipe )
        {
            TIMEPIPE_SERVICE()
                ->removeTimepipe( _timepipe );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}