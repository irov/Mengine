#	pragma once

#	include "Interface/AmplifierInterface.h"
#	include "Interface/SoundSystemInterface.h"

#	include "Core/ConstString.h"

#	include "Config/Typedef.h"

#	include <stdex/stl_map.h>

namespace	Menge
{
	class ResourcePlaylist;
	class Playlist;

	class Amplifier
		: public AmplifierInterface
		, public SoundListenerInterface	
	{
	public:
		Amplifier();
		~Amplifier();

	public:
		bool initialize() override;
		void finalize() override;

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

	public:
        bool playTrack( const ConstString& _playlistResource, size_t _index, float _pos, bool _looped ) override;
		void shuffle( const ConstString& _playlist ) override;
				
		void stop() override;
		void pause() override;
		void resume() override;
		
		const ConstString & getPlayTrack() const override;
		
		size_t getNumTracks() const override;
		size_t getCurrentTrack() const override;

		//void onTurnSound( bool _turn ) override;
		
		void setPosMs( float _posMs ) override;
		float getPosMs() const override;

	private:
        ServiceProviderInterface * m_serviceProvider;

		float m_volume;
		float m_volumeOverride;
		float m_currentSoundPosition;
		
		typedef	stdex::map<ConstString, Playlist *> TMapPlayList;
		TMapPlayList m_mapPlayLists;

		ConstString	m_currentPlaylistName;
		Playlist * m_currentPlayList;

		size_t m_sourceID;
		SoundBufferInterfacePtr m_buffer;

		bool m_play;
		bool m_needRefocus;
        bool m_turn;

    protected:
		void onSoundPause( size_t _id ) override;
		void onSoundStop( size_t _id ) override;


    protected:
		bool prepareSound_( const ConstString& _pakName, const FilePath& _file, const ConstString& _codecType, float _pos );
		bool loadPlayList_( const ConstString& _playlistResource );
		bool preparePlay_( float _pos );

		bool play2_( float _pos );
		bool play_();
	};
}
