#	include "ResourcePlaylist.h"

#	include "ResourceImplement.h"

#	include "XmlEngine.h"

#	include "FileEngine.h"

#	include "LogEngine.h"


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
	void ResourcePlaylist::loader( XmlElement * _xml )
	{
		ResourceReference::loader( _xml );

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE( "Loop", "Value", m_loop );

			XML_CASE_NODE("Track")
			{
				std::string filename;

				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "File", filename );
				}

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
	//////////////////////////////////////////////////////////////////////////
	bool ResourcePlaylist::_compile()
	{
		//if( Holder<XmlEngine>::hostage()
		//	->parseXml( m_filename, this ) == false )
		//{
		//	MENGE_LOG("Warning: resource playlist not find file '%s'\n"
		//		, m_filename.c_str() 
		//		);

		//	return false;
		//}

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