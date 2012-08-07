#	pragma once

#	include "ResourceReference.h"
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
		void setFilePath( const WString& _path );
		const WString& getFilePath() const;

		void setFolderPath( const WString& _path );
		const WString& getFolderPath() const;
		
	public:
		EmitterContainerInterface * getContainer() const;

	public:
		ResourceImageDefault* getAtlasImage( size_t _atlasId );

	public:
		void loader( const Metabuf::Metadata * _parser ) override;
	
	protected:
		bool _compile() override;
		void _release() override;

	protected:
		void createResource_( const ConstString & _fullname, const WString & _path );

	private:
		WString m_filename;
		WString m_folder;
		
		EmitterContainerInterface * m_container;

		typedef std::vector<ResourceImageDefault *> TVectorAtlasImages;
		TVectorAtlasImages m_atlasImages;
	};
}
