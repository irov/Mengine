#	pragma once

#	include "ResourceReference.h"

#	include <vector>

class FileDataInterface;

namespace Menge
{
	typedef	std::vector<std::string> TVecTrack;

	//! ResourcePlaylist - xml - файл, который хранит список имен треков и признак зациклинности. Имя трека это имя файла.

    /*! xml - файл имеет следующую структуру:
	*
	* <Tracks>
	*	<Loop Value = "1/0"/>
	*	<Track File = "имя_файла0"/>
	*  ...
	*	<Track File = "имя_файлаN"/>
	* </Tracks>
	*	
	*/
	class ResourcePlaylist
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourcePlaylist )

	public:
		//! Конструктор.
		/*!
		\param _name имя ресурса.
		*/
		ResourcePlaylist( const std::string & _name );

	public:
		//! Возвращает вектор треков.
		/*!
		\return вектор треков
		*/
		const TVecTrack & getTracks() const;

		//! Возвращает имя _track трека
		/*!
		\param _track индекс трека
		\return имя трека
		*/
		const std::string & getTrack( unsigned int _track ) const;

		//! Возвращает флаг зацикливания
		/*!
		\return флаг зацикливания
		*/
		bool getLoop() const;

	public:
		void loader( XmlElement * _xml ) override;
		void loaderTracks_( XmlElement * _xml );
		void loaderTrack_( XmlElement * _xml );

	protected:
		bool _compile() override;
		void _release() override;

	private:
		bool	  m_loop;
		TVecTrack m_tracks;

		std::string m_filename;
	};
}