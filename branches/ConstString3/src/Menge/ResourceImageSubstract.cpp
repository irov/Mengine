#	include "ResourceImageSubstract.h"
#	include "ResourceImplement.h"

#	include "ResourceManager.h"

#	include "Interface/ImageCodecInterface.h"

#	include "FileEngine.h"
#	include "CodecEngine.h"
#	include "LogEngine.h"
#	include "RenderEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceImageSubstract );
	//////////////////////////////////////////////////////////////////////////
	ResourceImageSubstract::ResourceImageSubstract()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	RenderTextureInterface* ResourceImageSubstract::getTexture() const
	{
		return 0;		
	}
}
