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
		ConstString type;
		ConstString locale;
				
		String platform;		
		WString filename;

		WString path;

		bool preload;
	};

	class ResourcePak
		: public Loadable
	{
	public:
		ResourcePak( const ResourcePakDesc & _desc, const WString & _baseDir );

	public:
		bool preload() const;

		const ConstString & getName() const;
		const ConstString & getLocale() const;
		const String & getPlatfrom() const;

		const WString & getPath() const;

	public:
		bool load();
		void apply();

	public:
		void loader( BinParser * _parser ) override;
		
	protected:
		void _loaded() override;

	protected:
		void loaderPak_( BinParser * _parser );
		void loaderScenes_( BinParser * _parser, const WString & _path );
		void loaderArrows_( BinParser * _parser, const WString & _path );
		void loaderEntities_( BinParser * _parser, const WString & _path );
		void loaderResources_( BinParser * _parser, const WString & _path );
		void loaderTexts_( BinParser * _parser, const WString & _path );

	protected:
		void addModulePath_( const String & _path );

	protected:
		void addScene_( const ConstString & _name, const WString & _path, bool _script );
		void addArrow_( const ConstString & _name, const WString & _path );
		void addEntity_( const ConstString & _name, const WString & _path );
		void addResource_( const ConstString & _name, const WString & _path );
		
		void addScriptPath_( const WString & _name );

		void addText_( const ConstString & _name, const WString & _path, const WString & _file );
		
	protected:
		typedef std::map<ConstString, ResourceDesc> TMapSceneDesc;
		TMapSceneDesc m_scenesDesc;

		typedef std::map<ConstString, ResourceDesc> TMapArrowDesc;
		TMapArrowDesc m_arrowsDesc;
		
		typedef std::map<ConstString, ResourceDesc> TMapPrototypeDesc;
		TMapPrototypeDesc m_prototypesDesc;

		typedef std::map<ConstString, ResourceDesc> TMapResourceDesc;
		TMapResourceDesc m_resourcesDesc;

		typedef std::map<ConstString, ResourceDesc> TMapTextDesc;
		TMapTextDesc m_textsDesc;
		
		ResourcePakDesc m_desc;
		WString m_baseDir;

		TVectorWString m_pathScripts;
	};
}