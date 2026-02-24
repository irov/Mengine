#pragma once

#include "Interface/ScriptServiceInterface.h"

#include "Kernel/FactorableUnique.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        bool addScriptEmbedding( const DocumentInterfacePtr & _doc )
        {
            if( SERVICE_IS_INITIALIZE( ScriptServiceInterface ) == false )
            {
                return false;
            }

            bool result = SCRIPT_SERVICE()
                ->addScriptEmbedding( T::getFactorableType(), Helper::makeFactorableUnique<T>( _doc ) );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        void removeScriptEmbedding()
        {
            if( SERVICE_IS_INITIALIZE( ScriptServiceInterface ) == false )
            {
                return;
            }

            SCRIPT_SERVICE()
                ->removeScriptEmbedding( T::getFactorableType() );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
