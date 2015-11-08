#	 pragma once

#	include "Interface/AmplifierInterface.h"
#	include "Interface/SoundSystemInterface.h"
#	include "Interface/MemoryInterface.h"

#	include "Core/ConstString.h"

#	include "Config/Typedef.h"

#	include <map>

namespace	Menge
{
	class ResourcePlaylist;
	class Playlist;

	class Amplifier
		: public ServiceBase<AmplifierInterface>
		, public SoundVolumeProviderInterface
	{
	public:
		Amplifier();
		~Amplifier();
		
	public:
		bool _initialize() override;
		void _finalize() override;

	public:
        bool playTrack( const ConstString& _playlistResource, uint32_t _index, float _pos, bool _looped ) override;
		bool shuffle( const ConstString& _playlist ) override;
				
		void stop() override;
		bool pause() override;
		bool resume() override;
		
		const ConstString& getPlayTrack() const override;
					
		uint32_t getNumTracks() const override;
		uint32_t getCurrentTrack() const override;

		//void onTurnSound( bool _turn ) override;
		
		void setPosMs( float _posMs ) override;
		float getPosMs() const override;

	protected:
		void onSoundChangeVolume( float _sound, float _music, float _voice ) override;

	protected:
        ServiceProviderInterface * m_serviceProvider;

		float m_volume;
		float m_volumeOverride;
		float m_currentSoundPosition;
		
		typedef	std::map<ConstString, Playlist *> TMapPlayList;
		TMapPlayList m_mapPlayLists;

		ConstString	m_currentPlaylistName;
		Playlist * m_currentPlayList;

		bool m_play;
		bool m_needRefocus;
        bool m_turn;

		MemoryInterfacePtr m_audioMemory;

	public:		
		void onSoundStop();

    protected:
		bool play_( const ConstString& _pakName, const FilePath& _filePath, const ConstString& _codec, bool _external, float _pos );

		bool loadPlayList_( const ConstString& _playlistResource );
		bool preparePlay_( float _pos );
	};
}
