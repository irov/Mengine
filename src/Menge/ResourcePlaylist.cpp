#	include "ResourcePlaylist.h"

#	include "ResourceImplement.h"

#	include "XmlEngine.h"

#	include "FileEngine.h"

#	include "LogEngine.h"

#	include "Utils.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourcePlaylist );
	//////////////////////////////////////////////////////////////////////////
	ResourcePlaylist::ResourcePlaylist( const ResourceFactoryParam & _params )
		: ResourceReference( _params )
		, m_loop( true )
		, m_shuffle( false )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePlaylist::setFilePath( const String& _path )
	{
		m_filename = m_params.category + _path;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePlaylist::loader( XmlElement * _xml )
	{
		ResourceReference::loader( _xml );

		//<Resource Name = "SoundGamePlay" Type = "ResourcePlaylist">
		//	<File Path = "Sounds/Music/GamePlay.xml"/>
		//	</Resource>

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE_METHOD( "File", "Path", &ResourcePlaylist::setFilePath );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePlaylist::loaderTracks_( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE( "Loop", "Value", m_loop );
			XML_CASE_ATTRIBUTE_NODE( "Shuffle", "Value", m_shuffle );

			XML_CASE_NODE("Tracks")
			{
				XML_PARSE_ELEMENT( this, &ResourcePlaylist::loaderTrack_ );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePlaylist::loaderTrack_( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE("Track")
			{
				std::string filename;

				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "File", filename );
				}

				std::string filepath = m_params.category + filename;

				if( Holder<FileEngine>::hostage()->existFile( filepath ) == false )
				{
					MENGE_LOG("ResourcePlaylist : %s not exist. \n", filepath.c_str() );
				}
				else
				{
					m_tracks.push_back( filepath );
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourcePlaylist::_compile()
	{
		if( Holder<XmlEngine>::hostage()
			->parseXmlFileM( m_filename, this, &ResourcePlaylist::loaderTracks_ ) == false )
		{
			MENGE_LOG("Warning: resource playlist not found file '%s'\n"
				, m_filename.c_str() 
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	const String& ResourcePlaylist::getTrack( unsigned int _track ) const
	{
		if( _track >= m_tracks.size() || _track < 0 )
		{
			return Utils::emptyString();
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
	bool ResourcePlaylist::getShuffle() const
	{
		return m_shuffle;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePlaylist::_release()
	{
	}
	//////////////////////////////////////////////////////////////////////////
}