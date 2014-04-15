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
		ResourcePak( ServiceProviderInterface * _serviceProvider
			, const FilePath & _baseDir
			, const ConstString & _name
			, const ConstString & _type
			, const ConstString & _locale
			, const ConstString & _platform
			, const FilePath & _descriptionPath
			, const FilePath & _path
			, bool _preload
			);

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
		bool mountFileGroup_();
		bool loadPak_();

	protected:
		void addModulePath_( const String & _path );

	protected:
		void addResource_( const FilePath & _path );
        void addTextPath_( const FilePath & _path );
		void addScriptPath_( const FilePath & _path );
		void addScriptModule_( const ConstString & _path );
		void addFontPath_( const FilePath & _font );

    protected:
        bool loadText_( const ConstString & _pakName, const FilePath & _path );
		bool loadFont_( const ConstString & _pakName, const FilePath & _path );
				
	protected:
        ServiceProviderInterface * m_serviceProvider;

		TVectorFilePath m_resourcesDesc;
				
		ConstString m_name;
		ConstString m_type;
		ConstString m_locale;

		ConstString m_platform;		
		FilePath m_descriptionPath;

        FilePath m_baseDir;
		FilePath m_path;

		TVectorFilePath m_pathScripts;
		TVectorConstString m_pathModules;

		TVectorFilePath m_pathFonts;
		TVectorFilePath m_pathTexts;

        bool m_preload;
	};
}