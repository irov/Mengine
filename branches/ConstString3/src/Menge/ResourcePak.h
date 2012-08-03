#	pragma once

#	include "ResourceManager.h"

#	include "Loadable.h"

#	include "Core/String.h"

#	include <map>

namespace Menge
{
	class Scene;
	class Arrow;
	class Entity;
	class Resource;

	class ResourcePak
		: public Loadable
	{
	public:
		ResourcePak( const ConstString & _name, const ConstString & _type, const ConstString & _locale, const String & _platform, const WString & _filename, const WString & _path, bool _preload, const WString & _baseDir );

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
		void loaderResources_( BinParser * _parser, const WString & _path );
		void loaderTexts_( BinParser * _parser, const WString & _path );

	protected:
		void addModulePath_( const String & _path );

	protected:
		void addResource_( const ConstString & _name, const WString & _path );
		
		void addScriptPath_( const WString & _name );

		void addText_( const ConstString & _name, const WString & _path, const WString & _file );
		
	protected:
		typedef std::vector<ResourceDesc> TResourceDescs;
		TResourceDescs m_resourcesDesc;

		typedef std::vector<ResourceDesc> TTextDescs;
		TTextDescs m_textsDesc;
		
		ConstString m_name;
		ConstString m_type;
		ConstString m_locale;

		String m_platform;		
		WString m_filename;

		WString m_path;

		bool m_preload;

		WString m_baseDir;

		TVectorWString m_pathScripts;
	};
}