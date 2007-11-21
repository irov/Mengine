#	include "ResourcePlaylist.h"
#	include "ResourceImplement.h"

#	include "FileEngine.h"
#	include "LogEngine.h"

#	include "XmlParser/XmlParser.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourcePlaylist )
	//////////////////////////////////////////////////////////////////////////
	ResourcePlaylist::ResourcePlaylist( const std::string & _name )
		: ResourceReference( _name )
		, m_loop( true )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePlaylist::loader( TiXmlElement * _xml )
	{
		ResourceReference::loader( _xml );

		XML_FOR_EACH_TREE( _xml )
		{
			XML_CHECK_VALUE_NODE( "File", "Path", m_filename );
			XML_CHECK_VALUE_NODE( "Loop", "Value", m_loop );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourcePlaylist::_compile()
	{
		TiXmlDocument * document = Holder<FileEngine>::hostage()
			->loadXml( m_filename );

		if( document == 0 )
		{
			MENGE_LOG("Warning: resource playlist not find file '%s'\n"
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
						MENGE_LOG("ResourcePlaylist : %s not exist. \n", filename.c_str() );
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
	const std::string & ResourcePlaylist::getTrack( size_t _track ) const
	{
		if( _track >= m_tracks.size() || _track < 0 )
		{
			static std::string empty;
			return empty;
		}
		
		return m_tracks[ _track ];
	}
	//////////////////////////////////////////////////////////////////////////
	const TVecTrack & ResourcePlaylist::getTracks() const
	{
		return m_tracks;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourcePlaylist::getLoop() const
	{
		return m_loop;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePlaylist::_release()
	{
	}
	//////////////////////////////////////////////////////////////////////////
}