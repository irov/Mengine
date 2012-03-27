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
		void setFilePath( const ConstString& _path );
		const ConstString& getFilePath() const;

		void setFolderPath( const ConstString& _path );
		const ConstString& getFolderPath() const;
		
		void setEmitterName( const ConstString& _name );
		const ConstString& getEmitterName() const;

		void setEmitterStartPosition( float _startPosition );
		float getEmitterStartPosition () const;

		void setEmitterPositionOffset( const mt::vec2f& _offset );
		const mt::vec2f& getEmitterPositionOffset() const;

		void setEmitterRelative( bool _relative );
		bool getEmitterRelative() const;

	public:
		EmitterContainerInterface * getContainer() const;

	public:
		ResourceImageDefault* getAtlasImage( size_t _atlasId );

	public:
		void loader( BinParser * _parser ) override;
	
	protected:
		bool _compile() override;
		void _release() override;

	protected:
		void createResource_( const ConstString & _fullname );

	private:
		ConstString m_filename;
		ConstString m_folder;
		
		//added for setup particle emitter at movie export 
		ConstString m_emitterName;
		float m_emitterStartPosition;
		mt::vec2f m_emitterPositionOffset;
		bool m_emitterRelative;

		EmitterContainerInterface * m_container;

		typedef std::vector<ResourceImageDefault *> TVectorAtlasImages;
		TVectorAtlasImages m_atlasImages;
	};
}
