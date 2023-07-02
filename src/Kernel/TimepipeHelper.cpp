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
        UniqueId addTimepipe( const LambdaTimepipe & _lambda, const DocumentInterfacePtr & _doc )
        {
            TimepipeInterfacePtr timepipe = Helper::makeFactorableUnique<Detail::MyTimepipe>( _doc, _lambda );

            UniqueId id = TIMEPIPE_SERVICE()
                ->addTimepipe( timepipe, _doc );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        void removeTimepipe( UniqueId _id )
        {
            TIMEPIPE_SERVICE()
                ->removeTimepipe( _id );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}