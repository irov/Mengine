#pragma once

#include "Interface/ScriptServiceInterface.h"
#include "Interface/ScriptEmbeddingInterface.h"

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

            const ConstString & type = T::getFactorableType();
            ScriptEmbeddingInterfacePtr embedding = Helper::makeFactorableUnique<T>( _doc );

            bool result = SCRIPT_SERVICE()
                ->addScriptEmbedding( type, embedding );

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

            const ConstString & type = T::getFactorableType();

            SCRIPT_SERVICE()
                ->removeScriptEmbedding( type );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
