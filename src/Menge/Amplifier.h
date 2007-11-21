#	 pragma once

#	include "Interface/SoundSystemInterface.h"

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
			void	play( const std::string & _playlist );

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

			void	update( float _timing );

		private:
			bool m_changeTrack;	// for FIX bug SQUALL :(

			float m_volume;

			typedef	std::map< std::string, Playlist* >	TPlayListMap;

			TPlayListMap	m_playLists;

			Playlist *	m_currentPlayList;
			
			SoundSourceInterface * m_music;
			SoundBufferInterface * m_buffer;

			void	listenPaused( bool _pause );
			void	listenStopped();
			void	release_();	
			void	play_();
	};
}