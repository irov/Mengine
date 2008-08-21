#	pragma once

#	include "ResourceReference.h"

#	include <map>

namespace Menge
{
	class EmitterContainerInterface;
	//class FileDataInterface;
	class RenderImageInterface;

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
		ResourceEmitterContainer( const ResourceFactoryParam & _params );

	public:
		void loader( XmlElement * _xml ) override;
		
		void setFilePath( const String& _path );
		void setFolderPath( const String& _path );

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
		RenderImageInterface * getRenderImage( const String& _name );

	protected:
		bool _compile() override;
		void _release() override;

	private:
		String m_filename;
		String m_folder;

		EmitterContainerInterface * m_container;

		typedef std::map<String, RenderImageInterface * > TMapImageEmitters;
		TMapImageEmitters	m_mapImageEmitters;
	};
}