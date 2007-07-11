#	include "ParticleResource.h"
#	include "ResourceImplement.h"

#	include "FileEngine.h"
#	include "RenderEngine.h"

#	include "XmlParser/XmlParser.h"

#	include "Interface/RenderSystemInterface.h"

namespace Menge
{
	RESOURCE_IMPLEMENT( ResourceParticle )
	//////////////////////////////////////////////////////////////////////////
	ResourceParticle::ResourceParticle( const std::string & _name )
		: ResourceImpl( _name )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceParticle::loader( TiXmlElement * _xml )
	{
		ResourceImpl::loader( _xml );

		XML_FOR_EACH_TREE( _xml )
		{
			XML_CHECK_VALUE_NODE( "Image", "Name", m_filename );
			XML_CHECK_VALUE_NODE( "emission", "Value", m_desc.emission );
			XML_CHECK_VALUE_NODE( "life", "Value", m_desc.life );
			XML_CHECK_VALUE_NODE( "dir", "Value", m_desc.dir );
			XML_CHECK_VALUE_NODE( "spread", "Value", m_desc.spread );
			XML_CHECK_VALUE_NODE( "v", "Value", m_desc.v );
			XML_CHECK_VALUE_NODE( "g", "Value", m_desc.g );
			XML_CHECK_VALUE_NODE( "r", "Value", m_desc.r );
			XML_CHECK_VALUE_NODE( "tan", "Value", m_desc.tan );
			XML_CHECK_VALUE_NODE( "size", "Value", m_desc.size );
			XML_CHECK_VALUE_NODE( "spin", "Value", m_desc.spin );
			XML_CHECK_VALUE_NODE( "startColor", "Value", m_desc.startColor );
			XML_CHECK_VALUE_NODE( "endColor", "Value", m_desc.endColor );
			XML_CHECK_VALUE_NODE( "varColor", "Value", m_desc.varColor );
		}		
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceParticle::_compile()
	{
		m_image = Holder<RenderEngine>::hostage()->loadImage(m_filename,true);
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceParticle::_release()
	{
		Holder<RenderEngine>::hostage()
			->releaseRenderImage(m_image);
	}
	//////////////////////////////////////////////////////////////////////////
/*	const ParticleDesc&	ResourceParticle::getDesc()
	{
		return m_desc;
	}*/
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterface * ResourceParticle::getImage()
	{
		return m_image;
	}
}