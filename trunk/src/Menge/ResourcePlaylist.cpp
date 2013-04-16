#	include "ResourcePlaylist.h"
#	include "Kernel/ResourceImplement.h"

#	include "Interface/FileSystemInterface.h"
#	include "Interface/CodecInterface.h"

#	include "Metacode.h"

#	include "Logger/Logger.h"

#	include "Core/String.h"
#	include "Core/File.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourcePlaylist );
	//////////////////////////////////////////////////////////////////////////
	ResourcePlaylist::ResourcePlaylist()
		: m_loop(true)
		, m_shuffle(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourcePlaylist::_loader( const Metabuf::Metadata * _meta )
	{
        const Metacode::Meta_DataBlock::Meta_ResourcePlaylist * metadata 
            = static_cast<const Metacode::Meta_DataBlock::Meta_ResourcePlaylist *>(_meta);

        metadata->get_Loop_Value( m_loop );
        metadata->get_Shuffle_Value( m_shuffle );

        const Metacode::Meta_DataBlock::Meta_ResourcePlaylist::TVectorMeta_Tracks & includes_tracks = metadata->get_IncludesTracks();

        for( Metacode::Meta_DataBlock::Meta_ResourcePlaylist::TVectorMeta_Tracks::const_iterator
            it = includes_tracks.begin(),
            it_end = includes_tracks.end();
        it != it_end;
        ++it )
        {
            const Metacode::Meta_DataBlock::Meta_ResourcePlaylist::Meta_Tracks & meta_tracks = *it;

            const Metacode::Meta_DataBlock::Meta_ResourcePlaylist::Meta_Tracks::TVectorMeta_Track & includes_track = meta_tracks.get_IncludesTrack();

            for( Metacode::Meta_DataBlock::Meta_ResourcePlaylist::Meta_Tracks::TVectorMeta_Track::const_iterator
                it = includes_track.begin(),
                it_end = includes_track.end();
            it != it_end;
            ++it )
            {
                const Metacode::Meta_DataBlock::Meta_ResourcePlaylist::Meta_Tracks::Meta_Track & meta_track = *it;

                TrackDesc desc;

                desc.volume = 1.f;

                meta_track.swap_File( desc.path );
                
                const ConstString & category = this->getCategory();

                if( FILE_SERVICE(m_serviceProvider)
                    ->existFile( category, desc.path, NULL ) == false )
                {
                    LOGGER_ERROR(m_serviceProvider)( "ResourcePlaylist::loaderTrack_: '%s' sound '%s' not exist"
                        , this->getName().c_str()
                        , desc.path.c_str() 
                        );

                    continue;
                }

                if( meta_track.swap_Codec( desc.codec ) == false )
                {
                    desc.codec = this->getCodec_( desc.path );
                }

                m_tracks.push_back( desc );
            }
        }

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	const TrackDesc * ResourcePlaylist::getTrack( unsigned int _track ) const
	{
		if( _track >= m_tracks.size() )
		{
			return 0;
		}

		return &m_tracks[_track];
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
}
