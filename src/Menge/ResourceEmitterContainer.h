#	pragma once

#	include "Kernel/ResourceReference.h"
#	include "Utils/Core/Polygon.h"
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
		void setFilePath( const FilePath& _path );
		const FilePath& getFilePath() const;

		void setFolderPath( const FilePath& _path );
		const FilePath& getFolderPath() const;
		
	public:
		EmitterContainerInterface * getContainer() const;

	public:
		ResourceImageDefault* getAtlasImage( size_t _atlasId );

	public:
		bool _loader( const Metabuf::Metadata * _parser ) override;
	
	protected:
		bool _compile() override;
		void _release() override;

	protected:
		void createResource_( const ConstString & _fullname, const FilePath & _path );

	private:
		FilePath m_filename;
		FilePath m_folder;
		
		EmitterContainerInterface * m_container;

		typedef std::vector<ResourceImageDefault *> TVectorAtlasImages;
		TVectorAtlasImages m_atlasImages;
	};
}
