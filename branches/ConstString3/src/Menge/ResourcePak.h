#	pragma once

#	include "Loadable.h"

#	include "ResourceManager.h"

#	include "Core/String.h"

#	include <map>

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
		ConstString type;
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
		void apply();

	public:
		void loader( BinParser * _parser ) override;
		
	protected:
		void _loaded() override;

	protected:
		void loaderPak_( BinParser * _parser );
		void loaderScenes_( BinParser * _parser, const String & _path );
		void loaderArrows_( BinParser * _parser, const String & _path );
		void loaderEntities_( BinParser * _parser, const String & _path );
		void loaderResources_( BinParser * _parser, const String & _path );
		void loaderParams_( BinParser * _parser, const String & _path );
		void loaderTexts_( BinParser * _parser, const String & _path );

	protected:
		void addModulePath_( const String & _path );

	protected:
		void addScene_( const ConstString & _name, const String & _path, bool _script );
		void addArrow_( const ConstString & _name, const String & _path );
		void addEntity_( const ConstString & _name, const String & _path );
		void addResource_( const ConstString & _name, const String & _path );
		void addParam_( const ConstString & _name, const String & _path );		

		void addScriptPath_( const String & _name );

		void addText_( const ConstString & _name, const String & _path, const String & _file );
		
	protected:
		typedef std::map<ConstString, ResourceDesc> TMapSceneDesc;
		TMapSceneDesc m_scenesDesc;

		typedef std::map<ConstString, ResourceDesc> TMapArrowDesc;
		TMapArrowDesc m_arrowsDesc;
		
		typedef std::map<ConstString, ResourceDesc> TMapPrototypeDesc;
		TMapPrototypeDesc m_prototypesDesc;

		typedef std::map<ConstString, ResourceDesc> TMapResourceDesc;
		TMapResourceDesc m_resourcesDesc;

		typedef std::map<ConstString, ResourceDesc> TMapParamDesc;
		TMapParamDesc m_paramsDesc;
			
		typedef std::map<ConstString, ResourceDesc> TMapTextDesc;
		TMapTextDesc m_textsDesc;


		ResourcePakDesc m_desc;
		String m_baseDir;

		TVectorString m_pathScripts;
	};
}