#	 pragma once

#	include "Interface/AmplifierServiceInterface.h"
#	include "Interface/SoundSystemInterface.h"

#	include "Core/ConstString.h"

#	include "Config/Typedef.h"

#	include <map>

namespace	Menge
{
	class ResourcePlaylist;
	class Playlist;

	class AmplifierService
		: public AmplifierServiceInterface
		, public SoundNodeListenerInterface	
	{
	public:
		AmplifierService();
		~AmplifierService();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

	public:
        void playTrack( const ConstString& _playlistResource, int _index, bool _looped ) override;
		void playAllTracks( const ConstString& _playlistResource ) override;
		void resetPlayList() override;
		void shuffle( const ConstString& _playlist ) override;

		void play() override;
		void stop() override;
		void pause() override;
		void resume() override;
		
		const ConstString& getPlayTrack() const override;
				
		void setVolume( float _value ) override;

		size_t getNumTracks() const override;

		void onTurnSound( bool _turn ) override;
		
		void setPosMs( float _posMs ) override;
		float getPosMs() const override;

	private:
        ServiceProviderInterface * m_serviceProvider;

		float m_volume;
		float m_volumeOverride;
		float m_currentSoundPosition;
		
		typedef	std::map<ConstString, Playlist *> TMapPlayList;

		TMapPlayList m_mapPlayLists;

		ConstString	m_currentPlaylistName;
		Playlist * m_currentPlayList;

		unsigned int m_sourceID;
		SoundBufferInterface * m_buffer;

		bool m_playing;
		bool m_needRefocus;

    protected:
		void listenSoundNodePaused() override;
		void listenSoundNodeStopped() override;


    protected:
		void release_();	
		void prepareSound_( const ConstString& _pakName, const FilePath& _file, const ConstString& _codec );
		bool loadPlayList_( const ConstString& _playlistResource );
		bool preparePlay_();
		void play_();
	};
}
