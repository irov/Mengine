#	pragma once

#	include "Loadable.h"

#	include "Core/String.h"

#	include "XmlEngine.h"

#	include <set>

namespace Menge
{
	class Scene;
	class Arrow;
	class Entity;
	class Resource;

	struct ResourcePakDesc
	{
		ConstString name;
		String path;
		String description;
		bool preload;
	};

	class ResourcePak
		: public Loadable
	{
	public:
		ResourcePak( const ResourcePakDesc & _desc, const String & _baseDir );

	public:
		bool preload() const;

		const ConstString & getName() const;
		const String & getPath() const;

	public:
		bool load();

	public:
		void loader( BinParser * _parser ) override;
		void loaded() override;

	protected:
		void loaderResources_( BinParser * _parser );
		void loaderScenes_( BinParser * _parser );
		void loaderArrows_( BinParser * _parser );
		void loaderEntities_( BinParser * _parser );
		void loaderResource_( BinParser * _parser );

	protected:
		void addModulePath_( const String & _path );

	protected:
		void addScene_( const ConstString & _name );
		void addArrow_( const ConstString & _name );
		void addEntity_( const ConstString & _name );
		void addResource_( const ConstString & _name );

		void addScriptPath_( const String & _name );

		void setTextsPath_( const String & _path );

	protected:
		ResourcePakDesc m_desc;
		String m_baseDir;

		String m_pathScenes;
		String m_pathArrows;
		String m_pathEntities;
		String m_pathResources;

		TVectorString m_pathScripts;
	};
}