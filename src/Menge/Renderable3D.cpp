#	include "Renderable3D.h"

#	include "XmlParser/XmlParser.h"

#	include "RenderEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Renderable3D::Renderable3D()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	bool Renderable3D::isRenderable()
	{
		if( NodeRenderable::isRenderable() == false )
		{
			return false;
		}

		Camera3D * camera =
			Holder<RenderEngine>::hostage()
			->getRenderCamera();

		if( isVisible( camera ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Renderable3D::isVisible( const Camera3D * _camera )
	{
		return true;
	}
}