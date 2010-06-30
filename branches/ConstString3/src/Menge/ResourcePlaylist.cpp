#	include "ResourcePlaylist.h"

#	include "ResourceImplement.h"

#	include "XmlEngine.h"

#	include "Logger/Logger.h"
#	include "Core/String.h"
#	include "Core/File.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourcePlaylist );
	//////////////////////////////////////////////////////////////////////////
	ResourcePlaylist::ResourcePlaylist()
		: m_loop( true )
		, m_shuffle( false )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePlaylist::setFilePath( const String& _path )
	{
		m_filename = _path;
	}
	//////////////////////////////////////////////////////////////////////////
	const String& ResourcePlaylist::getFilePath() const
	{
		return m_filename;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePlaylist::loader( XmlElement * _xml )
	{
		ResourceReference::loader( _xml );

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

			XML_CASE_NODE( "Tracks" )
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
			XML_CASE_NODE( "Track" )
			{
				TrackDesc desc;

				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "File", desc.path );
					XML_CASE_ATTRIBUTE( "Codec", desc.codec );
				}

				const ConstString & category = this->getCategory();

				if( FileEngine::get()->existFile( category, desc.path ) == false )
				{
					MENGE_LOG_ERROR( "ResourcePlaylist : '%s' not exist"
						, desc.path.c_str() 
						);
				}
				else
				{
					if( desc.codec.empty() == true )
					{
						String codecType;
						Utils::getFileExt( codecType, desc.path );
						codecType += "Sound";

						desc.codec = codecType;
					}

					m_tracks.push_back( desc );
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourcePlaylist::_compile()
	{
		const ConstString & category = this->getCategory();

		if( XmlEngine::get()
			->parseXmlFileM( category, m_filename, this, &ResourcePlaylist::loaderTracks_ ) == false )
		{
			MENGE_LOG_ERROR( "Warning: resource playlist not found file '%s'"
				, m_filename.c_str()
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	const TrackDesc * ResourcePlaylist::getTrack( unsigned int _track ) const
	{
		if( _track >= m_tracks.size() || _track < 0 )
		{
			return 0;
		}

		return &m_tracks[ _track ];
	}
	//////////////////////////////////////////////////////////////////////////
	const TVectorTrackDesc & ResourcePlaylist::getTracks() const
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
