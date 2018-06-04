#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/FileSystemInterface.h"

#include "Core/IntrusivePtr.h"
#include "Core/ConstString.h"
#include "Core/FilePath.h"

namespace Mengine
{	
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class RenderTextureInterface> RenderTextureInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
	class GraveyardInterface
		: public ServiceInterface
	{
        SERVICE_DECLARE("Graveyard")

	public:
		virtual void tick( float _time, float _timing ) = 0;

	public:
		virtual void clearTextures() = 0;

	public:
		virtual bool buryTexture( RenderTextureInterface * _texture ) = 0;
		virtual RenderTextureInterfacePtr resurrectTexture( const FileGroupInterfacePtr& _fileGroup, const FilePath & _path ) = 0;
	};

#   define GRAVEYARD_SERVICE()\
    ((Mengine::GraveyardInterface*)SERVICE_GET(Mengine::GraveyardInterface))
}

