#	pragma once

#	include "ResourceReference.h"

#	include <map>

namespace Menge
{
	class EmitterContainerInterface;
	class ResourceImageDefault;

	//! ResourceEmitterContainer - ресурс-файл контейнера эмиттеров, который заполняется из формата *.ptc от Astralax. ptc содержит только имена текстур, поэтому необходимо задавать в какой директории они находятся.

    /*! xml - файл имеет следующую структуру:
	* <Resource Name = "имя_ресурса" Type = "ResourceEmitterContainer" >
	*	<File Path = "имя_файла"/>
	*	<Folder Path = "папка_с_текстурами_партиклов"/>
	* </Resource>
	*/

	class ResourceEmitterContainer
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceEmitterContainer )

	public:
		//! Конструктор.
		/*!
		\param _name имя ресурса.
		*/
		ResourceEmitterContainer();

	public:
		void loader( XmlElement * _xml ) override;
		


	public:

		//! Возвращает контейнер эмиттеров.
		/*!
		\return контейнер эмиттеров
		*/
		const EmitterContainerInterface * getContainer() const;

		//! Возвращает текстуру по имени.
		/*!
		\param _name имя текстуры
		\return изображение
		*/
		ResourceImageDefault* getRenderImage( const String& _name );
		bool releaseRenderImage( ResourceImageDefault * _resourceImage );

		const String& getFilePath() const;
		
		const String& getFolderPath() const;

	protected:
		bool _compile() override;
		void _release() override;

	private:
		String m_filename;
		String m_folder;

		void setFilePath( const String& _path );
		void setFolderPath( const String& _path );

		EmitterContainerInterface * m_container;

		typedef std::map< String, ResourceImageDefault* > TMapImageEmitters;
		TMapImageEmitters	m_mapImageEmitters;
	};
}
