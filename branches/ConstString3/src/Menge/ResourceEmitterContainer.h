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
		ResourceEmitterContainer();

	public:
		void setFilePath( const ConstString& _path );
		const ConstString& getFilePath() const;

		void setFolderPath( const ConstString& _path );
		const ConstString& getFolderPath() const;
	
	public:
		const EmitterContainerInterface * getContainer() const;
		ResourceImageDefault* getRenderImage( const char * _name );

	public:
		void loader( XmlElement * _xml ) override;
	
	protected:
		bool _compile() override;
		void _release() override;

	private:
		ConstString m_filename;
		ConstString m_folder;

		EmitterContainerInterface * m_container;

		typedef std::map<const char *, ResourceImageDefault*> TMapImageEmitters;
		TMapImageEmitters m_mapImageEmitters;
	};
}
