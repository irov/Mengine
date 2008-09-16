#	 pragma once

#	include "Config/Typedef.h"

#	include "Interface/SoundSystemInterface.h"
#	include "ValueInterpolator.h"
#	include <map>

namespace	Menge
{
	class ResourcePlaylist;
	class Playlist;

	//! Amplifier - ����������� ������. 
	/*! 
	*
	*/

	class Amplifier
		:	public SoundNodeListenerInterface
	{
		public:
			//! �����������. ���������� ��������� 100%.
			/*!
			*/
			Amplifier();

			//! ����������.
			/*!
			*/
			~Amplifier();

			//! ������������ ���������. �������� �������� ��� ������.
			/*!
			\param _playlist ��� �������.
			*/
			void	playAllTracks( const String& _playlistResource );

			//! Shuffle ���������.
			/*!
			\param _playlist ��� ���������.
			*/
			void	shuffle( const String& _playlist );

			//! ��������� ������.
			/*!
			*/
			void	stop();

			//! ��������� ���������. �������� �� [0;1]
			/*!
			\param _volume �������� ���������.
			*/

			void	setVolume( float _volume );

			//! ���������� �������� ���������
			/*!
			\return �������� ���������
			*/
			float	getVolume() const;

			const String& getPlaying() const;

			void volumeTo( float _time, float _value );
			std::size_t getNumTracks() const;

			void update( float _timing );

			void playTrack( const String& _playlistResource, int _index, bool _looped );

			
		private:
			float m_volume;
			float m_volumeOverride;

			typedef	std::map< String, Playlist* >	TMapPlayList;

			TMapPlayList	m_mapPlayLists;

			String	m_currentPlaylistName;
			Playlist *	m_currentPlayList;
			
			SoundSourceInterface * m_music;
			SoundBufferInterface * m_buffer;

			void	listenPaused( bool _pause );
			void	listenStopped();
			void	_release();	
			void	_prepareSound( const String& _filename );
			bool	_loadPlayList( const String& _playlistResource );

			ValueInterpolator<float> m_volumeTo;
	};
}