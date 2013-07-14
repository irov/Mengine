#	pragma once

#   include "Interface/ResourceInterface.h"

#	include "Core/String.h"
#   include "Core/FilePath.h"

#	include <map>

namespace Menge
{
    struct ResourceDesc
    {
        ConstString pakName;
        ConstString pakType;

        FilePath pakPath;

        FilePath path;
        bool script;
    };

	class ResourcePak
	{
	public:
		ResourcePak( ServiceProviderInterface * _serviceProvider, const ConstString & _name, const ConstString & _type, const ConstString & _locale, const ConstString & _platform, const FilePath & _filename, const FilePath & _path, bool _preload, const FilePath & _baseDir );

	public:
		bool isPreload() const;

		const ConstString & getName() const;
		const ConstString & getLocale() const;
		const ConstString & getPlatfrom() const;

		const FilePath & getPath() const;

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

		ConstString m_platform;		
		FilePath m_filename;

        FilePath m_baseDir;
		FilePath m_path;

		TVectorFilePath m_pathScripts;

        bool m_preload;
	};
}