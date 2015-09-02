#	pragma once

#	include "Interface/ServiceInterface.h"
#	include "Interface/RenderSystemInterface.h"

#	include "Core/ConstString.h"
#	include "Core/FilePath.h"

namespace Menge
{	
	class GraveyardInterface
		: public ServiceInterface
	{
        SERVICE_DECLARE("Graveyard")

	public:
		virtual bool initialize() = 0;
		virtual void finalize() = 0;

	public:
		virtual void update( float _timing ) = 0;

	public:
		virtual void clearTextures() = 0;

	public:
		virtual bool buryTexture( RenderTextureInterface * _texture ) = 0;
		virtual RenderTextureInterfacePtr resurrectTexture( const ConstString& _pakName, const FilePath & _path ) = 0;
	};

#   define GRAVEYARD_SERVICE(serviceProvider)\
    ((Menge::GraveyardInterface*)SERVICE_GET(serviceProvider, Menge::GraveyardInterface))
}

