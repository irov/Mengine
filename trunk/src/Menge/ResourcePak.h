#	pragma once

#   include "Interface/ResourceInterface.h"

#	include "Core/String.h"
#   include "Core/FilePath.h"

#	include <map>

namespace Menge
{
	class ResourcePak
	{
	public:
		ResourcePak( ServiceProviderInterface * _serviceProvider, const ConstString & _name, const ConstString & _type, const ConstString & _locale, const String & _platform, const String & _filename, const String & _path, bool _preload, const String & _baseDir );

	public:
		bool isPreload() const;

		const ConstString & getName() const;
		const ConstString & getLocale() const;
		const String & getPlatfrom() const;

		const String & getPath() const;

	public:
		bool load();
		bool apply();

	protected:
		void addModulePath_( const String & _path );

	protected:
		void addResource_( const FilePath & _path );
        void addText_( const FilePath & _path );
		void addScriptPath_( const FilePath & _path );

    protected:
        bool loadText_( const ConstString & _pakName, const FilePath & _path );
				
	protected:
        ServiceProviderInterface * m_serviceProvider;

		typedef std::vector<ResourceDesc> TResourceDescs;
		TResourceDescs m_resourcesDesc;

		typedef std::vector<ResourceDesc> TTextDescs;
		TTextDescs m_textsDesc;
		
		ConstString m_name;
		ConstString m_type;
		ConstString m_locale;

		String m_platform;		
		String m_filename;

        String m_baseDir;
		String m_path;

		TVectorFilePath m_pathScripts;

        bool m_preload;
	};
}