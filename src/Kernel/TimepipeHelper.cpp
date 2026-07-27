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
        TimepipeInterfacePtr addTimepipe( const LambdaTimepipeEvent & _lambda, const DocumentInterfacePtr & _doc )
        {
            LambdaTimepipePtr timepipe = Helper::makeFactorableUnique<Mengine::LambdaTimepipe>( _doc );
            timepipe->initialize( _lambda );

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
