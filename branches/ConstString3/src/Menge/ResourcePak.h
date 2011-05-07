#	pragma once

#	include "Loadable.h"

#	include "Core/String.h"

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
		
	protected:
		void _loaded() override;

	protected:
		void loaderPak_( BinParser * _parser );
		void loaderScenes_( BinParser * _parser, const ConstString & _path );
		void loaderArrows_( BinParser * _parser, const ConstString & _path );
		void loaderEntities_( BinParser * _parser, const ConstString & _path );
		void loaderResources_( BinParser * _parser, const String & _path );
		void loaderParams_( BinParser * _parser, const String & _path );

	protected:
		void addModulePath_( const String & _path );

	protected:
		void addScene_( const ConstString & _name, const ConstString & _path );
		void addArrow_( const ConstString & _name, const ConstString & _path );
		void addEntity_( const ConstString & _name, const ConstString & _path );
		void addResource_( const ConstString & _name, const String & _path );
		void addParam_( const ConstString & _name, const String & _path );		

		void addScriptPath_( const String & _name );

		void setTextsPath_( const String & _path );
		
	protected:
		ResourcePakDesc m_desc;
		String m_baseDir;

		TVectorString m_pathScripts;
	};
}