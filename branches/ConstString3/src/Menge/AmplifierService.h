#	 pragma once

#	include "Core/ConstString.h"

#	include "Config/Typedef.h"

#	include "Interface/SoundSystemInterface.h"
#	include "Interface/AmplifierServiceInterface.h"

#	include <map>

namespace	Menge
{
	class ResourcePlaylist;
	class Playlist;

	//! Amplifier - проигрывает музыку. 
	/*! 
	*
	*/

	class AmplifierService
		: public AmplifierServiceInterface
		, public SoundNodeListenerInterface	
	{
	public:
		AmplifierService();
		~AmplifierService();

	public:
		void playAllTracks( const ConstString& _playlistResource );
		void resetPlayList();
		void shuffle( const ConstString& _playlist );

		void play();
		void stop();
		void pause();
		void resume();
		
		const ConstString& getPlayTrack() const;
				
		void setVolume( float _value );

		size_t getNumTracks() const;

		void onTurnSound( bool _turn );

		void playTrack( const ConstString& _playlistResource, int _index, bool _looped );
				
		void setPosMs( float _posMs );
		float getPosMs() const;

	private:
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

	private:
		void	listenSoundNodePaused();
		void	listenSoundNodeStopped();
		void	release_();	
		void	prepareSound_( const ConstString& _pakName, const WString& _file, const ConstString& _codec );
		bool	loadPlayList_( const ConstString& _playlistResource );
		bool	preparePlay_();
		void	play_();
	};
}
