#	include "ResourceImageSet.h"
#	include "ResourceImplement.h"

#	include "FileEngine.h"
#	include "RenderEngine.h"

#	include "XmlParser/XmlParser.h"

namespace Menge
{
	RESOURCE_IMPLEMENT( ResourceImageSet )
	//////////////////////////////////////////////////////////////////////////
	ResourceImageSet::ResourceImageSet( const std::string & _name )
		: ResourceImageDefault( _name )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ResourceImageSet::getCount()
	{
		return m_uvs.size();
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f & ResourceImageSet::getUV( size_t _index )
	{
		return m_uvs[ _index ];
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageSet::loader( TiXmlElement * _xml )
	{
		ResourceImageDefault::loader( _xml );

		mt::vec4f val;

		XML_FOR_EACH_TREE( _xml )
		{
			XML_CHECK_NODE( "Image" )
			{
				XML_VALUE_ATTRIBUTE("UV", val);
				m_uvs.push_back( val );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageSet::_compile()
	{
		if( ResourceImageDefault::_compile() == false )
		{
			return false;
		}

		return true;
	}
}