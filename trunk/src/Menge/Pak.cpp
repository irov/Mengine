#	include "Pak.h"

#	include "Interface/LoaderInterface.h"
#	include "Interface/FileSystemInterface.h"
#	include "Interface/ScriptSystemInterface.h"
#	include "Interface/TextInterface.h"
#	include "Interface/StringizeInterface.h"
#	include "Interface/ResourceInterface.h"
#	include "Interface/UnicodeInterface.h"
#	include "Interface/ProfilerInterface.h"

#	include "Metacode/Metacode.h"

#   include "Logger/Logger.h"

#	include "Consts.h"

#   include <stdio.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Pak::Pak()
		: m_serviceProvider(nullptr)
		, m_preload(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Pak::setup( ServiceProviderInterface * _serviceProvider
		, const ConstString & _name
		, const ConstString & _type
		, const ConstString & _locale
		, const ConstString & _platform
		, const ConstString & _descriptionPath
		, const ConstString & _path
		, bool _preload	)
	{
		m_serviceProvider = _serviceProvider;
        m_name = _name;
		m_type = _type;
		m_locale = _locale;
		m_platform = _platform;
		m_descriptionPath = _descriptionPath;
		m_path = _path;
		m_preload = _preload;
	}
	//////////////////////////////////////////////////////////////////////////
	void Pak::setPreload( bool _value )
	{
		m_preload = _value;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Pak::isPreload() const
	{
		return m_preload;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & Pak::getName() const
	{
		return m_name;
	}
	//////////////////////////////////////////////////////////////////////////
	void Pak::setLocale( const ConstString & _locale )
	{
		m_locale = _locale;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & Pak::getLocale() const
	{
		return m_locale;
	}
	//////////////////////////////////////////////////////////////////////////
	void Pak::setPlatfrom( const ConstString & _platform )
	{
		m_platform = _platform;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & Pak::getPlatfrom() const
	{
		return m_platform;
	}
	//////////////////////////////////////////////////////////////////////////
	void Pak::setPath( const ConstString & _path )
	{
		m_path = _path;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & Pak::getPath() const
	{
		return m_path;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Pak::load()
	{		
		if( this->mountFileGroup_() == false )
		{
			return false;
		}

		if( this->loadPak_() == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Pak::mountFileGroup_()
	{
		if( m_path.empty() == true )
		{
			return true;
		}

		//FilePath fullPath = concatenationFilePath(m_serviceProvider, m_baseDir, m_path);
		
		if( FILE_SERVICE(m_serviceProvider)->mountFileGroup( m_name, m_path, m_type ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourcePak::load failed to mount pak '%s' path '%s'"
				, m_name.c_str()
				//, m_baseDir.c_str()
				, m_path.c_str()
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Pak::loadPak_()
	{
		if( m_descriptionPath.empty() == true )
		{
			return true;
		}

		Metacode::Meta_Pak pak;

		bool exist = false;
		if( LOADER_SERVICE(m_serviceProvider)->load( m_name, m_descriptionPath, &pak, exist ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourcePak::load Invalid resource file '%s:%s' '%s'"
				, m_path.c_str()
				, m_name.c_str()
				, m_descriptionPath.c_str()
				);

			return false;
		}

        m_resourcesDesc.reserve( 32 );        

		const Metacode::Meta_Pak::TVectorMeta_Scripts & includes_scripts = pak.get_IncludesScripts();

		for( Metacode::Meta_Pak::TVectorMeta_Scripts::const_iterator
			it = includes_scripts.begin(),
			it_end = includes_scripts.end();
		it != it_end;
		++it )
		{
			const Metacode::Meta_Pak::Meta_Scripts & scripts = *it;

			ConstString Path;
			scripts.swap_Path( Path );

			ConstString Module;
			scripts.swap_Module( Module );

			ConstString Initializer;
			scripts.swap_Initializer( Initializer );

			this->addScriptPak_( Path, Module, Initializer );
		}

		const Metacode::Meta_Pak::TVectorMeta_Fonts & includes_fonts = pak.get_IncludesFonts();

		for( Metacode::Meta_Pak::TVectorMeta_Fonts::const_iterator
			it = includes_fonts.begin(),
			it_end = includes_fonts.end();
		it != it_end;
		++it )
		{
			const Metacode::Meta_Pak::Meta_Fonts & fonts = *it;

			ConstString Path;
			fonts.swap_Path( Path );
			this->addFontPath_( Path );
		}

		const Metacode::Meta_Pak::TVectorMeta_Resources & includes_resources = pak.get_IncludesResources();

		for( Metacode::Meta_Pak::TVectorMeta_Resources::const_iterator
			it = includes_resources.begin(),
			it_end = includes_resources.end();
		it != it_end;
		++it )
		{
			const Metacode::Meta_Pak::Meta_Resources & meta_resources = *it;

			const Metacode::Meta_Pak::Meta_Resources::TVectorMeta_Resource & includes_resource = meta_resources.get_IncludesResource();
            
			for( Metacode::Meta_Pak::Meta_Resources::TVectorMeta_Resource::const_iterator
				it = includes_resource.begin(),
				it_end = includes_resource.end();
			it != it_end;
			++it )
			{
				const Metacode::Meta_Pak::Meta_Resources::Meta_Resource & meta_resource = *it;

				ConstString Path;
				meta_resource.swap_Path( Path );
				this->addResource_( Path );
			}
		}

		const Metacode::Meta_Pak::TVectorMeta_Texts & includes_tests = pak.get_IncludesTexts();

		for( Metacode::Meta_Pak::TVectorMeta_Texts::const_iterator
			it = includes_tests.begin(),
			it_end = includes_tests.end();
		it != it_end;
		++it )
		{
			const Metacode::Meta_Pak::Meta_Texts & meta_texts = *it;

			const Metacode::Meta_Pak::Meta_Texts::TVectorMeta_Text & includes_text = meta_texts.get_IncludesText();

			for( Metacode::Meta_Pak::Meta_Texts::TVectorMeta_Text::const_iterator
				it = includes_text.begin(),
				it_end = includes_text.end();
			it != it_end;
			++it )
			{
				const Metacode::Meta_Pak::Meta_Texts::Meta_Text & meta_text = *it;

				ConstString Path;
                meta_text.swap_Path( Path );
				this->addTextPath_( Path );
			}
		}

		const Metacode::Meta_Pak::TVectorMeta_Datas & includes_datas = pak.get_IncludesDatas();

		for( Metacode::Meta_Pak::TVectorMeta_Datas::const_iterator
			it = includes_datas.begin(),
			it_end = includes_datas.end();
		it != it_end;
		++it )
		{
			const Metacode::Meta_Pak::Meta_Datas & meta_datas = *it;

			const Metacode::Meta_Pak::Meta_Datas::TVectorMeta_Data & includes_data = meta_datas.get_IncludesData();

			for( Metacode::Meta_Pak::Meta_Datas::TVectorMeta_Data::const_iterator
				it = includes_data.begin(),
				it_end = includes_data.end();
			it != it_end;
			++it )
			{
				const Metacode::Meta_Pak::Meta_Datas::Meta_Data & meta_data = *it;

				const ConstString & name = meta_data.get_Name();
				const ConstString & path = meta_data.get_Path();

				this->addData_( name, path );
			}
		}

		SCRIPT_SERVICE(m_serviceProvider)
            ->addModulePath( m_name, m_scriptsPak );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Pak::apply()
	{
		for( TVectorConstString::const_iterator
			it = m_resourcesDesc.begin(),
			it_end = m_resourcesDesc.end();
		it != it_end;
		++it )
		{
            //PROFILER_SERVICE(m_serviceProvider)
            //    ->memoryBegin();

            const ConstString & path = *it;

			if( RESOURCE_SERVICE(m_serviceProvider)
				->loadResource( m_name, path ) == false )
            {
                return false;
            }

            //size_t memory = PROFILER_SERVICE(m_serviceProvider)
            //    ->memoryEnd();

            //LOGGER_ERROR(m_serviceProvider)("resource desc %.04f -- %s"
            //    , float(memory) / (1024.f)
            //    , desc.path.c_str()                
            //    );
		}

		for( TVectorConstString::iterator
			it = m_pathFonts.begin(),
			it_end = m_pathFonts.end();
		it != it_end;
		++it )
		{
			const ConstString & path = *it;

			if( this->loadFont_( m_name, path ) == false )
			{
				return false;
			}
		}

		for( TVectorConstString::iterator
			it = m_pathTexts.begin(),
			it_end = m_pathTexts.end();
		it != it_end;
		++it )
		{
            const ConstString & path = *it;

			if( this->loadText_( m_name, path ) == false )
            {
                return false;
            }
		}

		for( TVectorPakDataDesc::iterator
			it = m_datas.begin(),
			it_end = m_datas.end();
		it != it_end;
		++it )
		{
			const PakDataDesc & desc = *it;

			if( this->loadData_( m_name, desc.name, desc.path ) == false )
			{
				return false;
			}
		}

        return true;
	}
    //////////////////////////////////////////////////////////////////////////
    bool Pak::loadText_( const ConstString & _pakName, const ConstString & _path )
    {
        bool result = TEXT_SERVICE(m_serviceProvider)
			->loadTextEntry( m_locale, _pakName, _path );

        return result;
    }
	//////////////////////////////////////////////////////////////////////////
	bool Pak::loadFont_( const ConstString & _pakName, const ConstString & _path )
	{
		bool result = TEXT_SERVICE(m_serviceProvider)
			->loadFonts( m_locale, _pakName, _path );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Pak::loadData_( const ConstString & _pakName, const ConstString & _name, const FilePath & _path )
	{
		DataDesc data;
		data.category = _pakName;
		data.path = _path;

		bool result = GAME_SERVICE(m_serviceProvider)
			->addData( _name, data );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void Pak::addResource_( const ConstString & _path )
	{
		m_resourcesDesc.push_back( _path );
	}
	//////////////////////////////////////////////////////////////////////////
	void Pak::addTextPath_( const ConstString & _path )
	{
		m_pathTexts.push_back( _path );
	}
	//////////////////////////////////////////////////////////////////////////
	void Pak::addScriptPak_( const ConstString & _path, const ConstString & _module, const ConstString & _initializer )
	{
		ScriptModulePak pak;
		pak.path = _path;
		pak.module = _module;
		pak.initializer = _initializer;

		m_scriptsPak.push_back( pak );
	}
	//////////////////////////////////////////////////////////////////////////
	void Pak::addFontPath_( const ConstString & _font )
	{
		m_pathFonts.push_back( _font );
	}
	//////////////////////////////////////////////////////////////////////////
	void Pak::addData_( const ConstString & _name, const ConstString & _path )
	{
		PakDataDesc desc;
		desc.name = _name;
		desc.path = _path;

		m_datas.push_back( desc );
	}
}