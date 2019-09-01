#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/FileGroupInterface.h"
#include "Interface/UpdationInterface.h"
#include "Interface/RenderTextureInterface.h"

#include "Kernel/IntrusivePtr.h"
#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"

namespace Mengine
{
    class GraveyardServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "Graveyard" )

    public:
        virtual void tick( const UpdateContext * _context ) = 0;

    public:
        virtual void clearTextures() = 0;

    public:
        virtual bool buryTexture( RenderTextureInterface * _texture ) = 0;
        virtual RenderTextureInterfacePtr resurrectTexture( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const Char * _doc ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define GRAVEYARD_SERVICE()\
    ((Mengine::GraveyardServiceInterface*)SERVICE_GET(Mengine::GraveyardServiceInterface))
//////////////////////////////////////////////////////////////////////////

