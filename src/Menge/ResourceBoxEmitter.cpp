#	include "ResourceBoxEmitter.h"

#	include "ResourceImplement.h"

#	include "FileEngine.h"

#	include "RenderEngine.h"

#	include "XmlParser/XmlParser.h"

#	include "Interface/RenderSystemInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceBoxEmitter )
	//////////////////////////////////////////////////////////////////////////
	ResourceBoxEmitter::ResourceBoxEmitter( const std::string & _name )
		: ResourceImpl( _name )
	{}
	//////////////////////////////////////////////////////////////////////////
	void ResourceBoxEmitter::loader( TiXmlElement * _xml )
	{
		ResourceImpl::loader( _xml );

		XML_FOR_EACH_TREE( _xml )
		{
			XML_CHECK_VALUE_NODE( "position", "Value", m_desc.position );
			XML_CHECK_VALUE_NODE( "life", "Value", m_desc.life );
			XML_CHECK_VALUE_NODE( "velocity", "Value", m_desc.velocity );
			XML_CHECK_VALUE_NODE( "dir", "Value", m_desc.dir );
			XML_CHECK_VALUE_NODE( "spread", "Value", m_desc.spread );
		}	
	}
	//////////////////////////////////////////////////////////////////////////
	BoxEmitterDesc& ResourceBoxEmitter::getDesc()
	{
		return m_desc;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceBoxEmitter::_compile()
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceBoxEmitter::_release()
	{
		//Holder<RenderEngine>::hostage()
		//	->releaseRenderImage(m_image);
	}
}