#	include "ResourcePlaylist.h"
#	include "ResourceImplement.h"

#	include "FileEngine.h"

#	include "XmlParser/XmlParser.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourcePlaylist )
	//////////////////////////////////////////////////////////////////////////
	ResourcePlaylist::ResourcePlaylist( const std::string & _name )
		: ResourceReference( _name )
		, m_looped( true )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePlaylist::loader( TiXmlElement * _xml )
	{
		ResourceReference::loader( _xml );

		XML_FOR_EACH_TREE( _xml )
		{
			XML_CHECK_VALUE_NODE( "File", "Path", m_filename );
			XML_CHECK_VALUE_NODE( "Loop", "Value", m_looped );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourcePlaylist::getLooped() const
	{
		return m_looped;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourcePlaylist::_compile()
	{
		TiXmlDocument * document = Holder<FileEngine>::hostage()
			->loadXml( m_filename );

		if( document == 0 )
		{
			printf("Warning: resource playlist not find file '%s'\n"
				, m_filename.c_str() 
				);

			return false;
		}

		std::string	filename;

		XML_FOR_EACH_DOCUMENT( document )
		{
			XML_CHECK_NODE_FOR_EACH("Tracks")
			{
				XML_CHECK_NODE("Track")
				{
					XML_VALUE_ATTRIBUTE( "File", filename );

					if( Holder<FileEngine>::hostage()->existFile( filename ) == false )
					{
						printf("ResourcePlaylist : %s not exist. \n", filename.c_str() );
					}
					else
					{
						m_tracks.push_back( filename );
					}
				}
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	const std::vector<std::string> & ResourcePlaylist::getTracks() const
	{
		return m_tracks;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePlaylist::_release()
	{
	}
	//////////////////////////////////////////////////////////////////////////
}