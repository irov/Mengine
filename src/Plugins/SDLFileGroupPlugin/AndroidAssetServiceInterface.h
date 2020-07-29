#pragma once

#include "Interface/ServiceInterface.h"

#include "Config/Char.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AndroidAssetServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AndroidAssetService" )

    public:
        virtual bool existFile( const Char * _filePath ) const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define ANDROIDASSET_SERVICE()\
    ((Mengine::AndroidAssetServiceInterface*)SERVICE_GET(Mengine::AndroidAssetServiceInterface))
//////////////////////////////////////////////////////////////////////////