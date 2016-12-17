#	include "Package.h"

#	include "Interface/LoaderInterface.h"
#	include "Interface/FileSystemInterface.h"
#	include "Interface/TextInterface.h"
#	include "Interface/StringizeInterface.h"
#	include "Interface/ResourceInterface.h"
#	include "Interface/UnicodeInterface.h"
#	include "Interface/ProfilerInterface.h"
#	include "Interface/UserdataInterface.h"
#	include "Interface/PlatformInterface.h"

#	include "Metacode/Metacode.h"

#   include "Logger/Logger.h"

#   include <stdio.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Package::Package()
		: m_serviceProvider(nullptr)
		, m_preload(false)
		, m_load(false)
		, m_enable(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Package::setup( ServiceProviderInterface * _serviceProvider
		, const ConstString & _name
		, const ConstString & _type
		, const ConstString & _locale
		, const Tags & _platform
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
	void Package::setPreload( bool _value )
	{
		m_preload = _value;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Package::isPreload() const
	{
		return m_preload;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & Package::getName() const
	{
		return m_name;
	}
	//////////////////////////////////////////////////////////////////////////
	void Package::setLocale( const ConstString & _locale )
	{
		m_locale = _locale;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & Package::getLocale() const
	{
		return m_locale;
	}
	//////////////////////////////////////////////////////////////////////////
	void Package::setPlatfromTags( const Tags & _platform )
	{
		m_platform = _platform;
	}
	//////////////////////////////////////////////////////////////////////////
	const Tags & Package::getPlatfromTags() const
	{
		return m_platform;
	}
	//////////////////////////////////////////////////////////////////////////
	void Package::setPath( const ConstString & _path )
	{
		m_path = _path;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & Package::getPath() const
	{
		return m_path;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Package::load()
	{		
		if( this->mountFileGroup_() == false )
		{
			return false;
		}

		if( this->loadPak_() == false )
		{
			return false;
		}

		m_load = true;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Package::isLoad() const
	{
		return m_load;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Package::mountFileGroup_()
	{	
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
	bool Package::loadPak_()
	{
		if( SERVICE_EXIST( m_serviceProvider, LoaderServiceInterface ) == false )
		{
			return false;
		}

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
			const Metacode::Meta_Pak::Meta_Scripts & meta_scripts = *it;

			const FilePath & Path = meta_scripts.get_Path();
			
			ConstString Module;
			meta_scripts.swap_Module( Module );

			ConstString Initializer;
			meta_scripts.swap_Initializer( Initializer );

			ConstString Finalizer;
			meta_scripts.swap_Finalizer( Finalizer );

			Tags Platform;
			meta_scripts.swap_Platform( Platform );

			this->addScriptPak_( Path, Module, Initializer, Finalizer, Platform );
		}

		const Metacode::Meta_Pak::TVectorMeta_Fonts & includes_fonts = pak.get_IncludesFonts();

		for( Metacode::Meta_Pak::TVectorMeta_Fonts::const_iterator
			it = includes_fonts.begin(),
			it_end = includes_fonts.end();
		it != it_end;
		++it )
		{
			const Metacode::Meta_Pak::Meta_Fonts & meta_fonts = *it;

			const FilePath & Path = meta_fonts.get_Path();

			Tags Platform;
			meta_fonts.swap_Platform( Platform );

			this->addFontPath_( Path, Platform );
		}

		const Metacode::Meta_Pak::TVectorMeta_Resources & includes_resources = pak.get_IncludesResources();

		for( Metacode::Meta_Pak::TVectorMeta_Resources::const_iterator
			it = includes_resources.begin(),
			it_end = includes_resources.end();
		it != it_end;
		++it )
		{
			const Metacode::Meta_Pak::Meta_Resources & meta_resources = *it;

			bool ignored = false;
			meta_resources.get_Ignored( ignored );

			Tags platform;
			meta_resources.get_Platform( platform );

			const Metacode::Meta_Pak::Meta_Resources::TVectorMeta_Resource & includes_resource = meta_resources.get_IncludesResource();
            
			for( Metacode::Meta_Pak::Meta_Resources::TVectorMeta_Resource::const_iterator
				it_include = includes_resource.begin(),
				it_include_end = includes_resource.end();
			it_include != it_include_end;
			++it_include)
			{
				const Metacode::Meta_Pak::Meta_Resources::Meta_Resource & meta_resource = *it_include;

				const FilePath & Path = meta_resource.get_Path();
				this->addResource_( Path, platform, ignored );
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

			Tags platform;
			meta_texts.get_Platform( platform );

			const Metacode::Meta_Pak::Meta_Texts::TVectorMeta_Text & includes_text = meta_texts.get_IncludesText();

			for( Metacode::Meta_Pak::Meta_Texts::TVectorMeta_Text::const_iterator
				it_include = includes_text.begin(),
				it_include_end = includes_text.end();
				it_include != it_include_end;
			++it_include)
			{
				const Metacode::Meta_Pak::Meta_Texts::Meta_Text & meta_text = *it_include;

				const FilePath & Path = meta_text.get_Path();
				this->addTextPath_( Path, platform );
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

			Tags platform;
			meta_datas.get_Platform( platform );

			const Metacode::Meta_Pak::Meta_Datas::TVectorMeta_Data & includes_data = meta_datas.get_IncludesData();

			for( Metacode::Meta_Pak::Meta_Datas::TVectorMeta_Data::const_iterator
				it_include = includes_data.begin(),
				it_include_end = includes_data.end();
				it_include != it_include_end;
			++it_include)
			{
				const Metacode::Meta_Pak::Meta_Datas::Meta_Data & meta_data = *it_include;

				const ConstString & name = meta_data.get_Name();
				const ConstString & path = meta_data.get_Path();

				this->addData_( name, path, platform );
			}
		}

		const Metacode::Meta_Pak::TVectorMeta_Materials & includes_materials = pak.get_IncludesMaterials();

		for( Metacode::Meta_Pak::TVectorMeta_Materials::const_iterator
			it = includes_materials.begin(),
			it_end = includes_materials.end();
		it != it_end;
		++it )
		{
			const Metacode::Meta_Pak::Meta_Materials & meta_materials = *it;

			Tags platform;
			meta_materials.get_Platform( platform );

			const Metacode::Meta_Pak::Meta_Materials::TVectorMeta_Material & includes_material = meta_materials.get_IncludesMaterial();

			for( Metacode::Meta_Pak::Meta_Materials::TVectorMeta_Material::const_iterator
				it_include = includes_material.begin(),
				it_include_end = includes_material.end();
				it_include != it_include_end;
			++it_include)
			{
				const Metacode::Meta_Pak::Meta_Materials::Meta_Material & meta_material = *it_include;

				const FilePath & path = meta_material.get_Path();

				this->addMaterial_( path, platform );
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Package::enable()
	{
		if( m_enable == true )
		{
			LOGGER_ERROR( m_serviceProvider )("Package::enable already enable '%s:%s' '%s'"
				, m_path.c_str()
				, m_name.c_str()
				, m_descriptionPath.c_str()
				);

			return false;
		}

		const Tags & platformTags = PLATFORM_SERVICE( m_serviceProvider )
			->getPlatformTags();

		SCRIPT_SERVICE( m_serviceProvider )
			->addModulePath( m_name, m_scriptsPackages );

		for( TVectorPakResourceDesc::const_iterator
			it = m_resourcesDesc.begin(),
			it_end = m_resourcesDesc.end();
		it != it_end;
		++it )
		{
            //PROFILER_SERVICE(m_serviceProvider)
            //    ->memoryBegin();

			const PakResourceDesc & desc = *it;

			if( RESOURCE_SERVICE(m_serviceProvider)
				->loadResources( m_locale, m_name, desc.path, desc.ignored ) == false )
            {
                LOGGER_ERROR( m_serviceProvider )("Package::enable '%s:%s' invalid load resource '%s'"
                    , m_path.c_str()
                    , m_name.c_str()
                    , desc.path.c_str()
                    );

                return false;
            }

			//size_t memory = PROFILER_SERVICE( m_serviceProvider )
			//	->memoryEnd();

			//LOGGER_ERROR( m_serviceProvider )("resource desc %.04f -- %s"
			//	, float( memory ) / (1024.f)
			//	, desc.path.c_str()
			//	);
		}

		for( TVectorPakFontDesc::const_iterator
			it = m_pathFonts.begin(),
			it_end = m_pathFonts.end();
		it != it_end;
		++it )
		{
			const PakFontDesc & desc = *it;

			if( this->loadFont_( m_name, desc.path ) == false )
			{
                LOGGER_ERROR( m_serviceProvider )("Package::enable '%s:%s' invalid load font '%s'"
                    , m_path.c_str()
                    , m_name.c_str()
					, desc.path.c_str()
                    );

				return false;
			}
		}

		for( TVectorPakTextDesc::iterator
			it = m_pathTexts.begin(),
			it_end = m_pathTexts.end();
		it != it_end;
		++it )
		{
			const PakTextDesc & desc = *it;

			if( this->loadText_( m_name, desc.path ) == false )
            {
                LOGGER_ERROR( m_serviceProvider )("Package::enable '%s:%s' invalid load text '%s'"
                    , m_path.c_str()
                    , m_name.c_str()
					, desc.path.c_str()
                    );

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

			if( this->addUserData_( m_name, desc.name, desc.path ) == false )
			{
                LOGGER_ERROR( m_serviceProvider )("Package::enable '%s:%s' invalid load userdata '%s' path '%s'"
                    , m_path.c_str()
                    , m_name.c_str()
                    , desc.name.c_str()
                    , desc.path.c_str()
                    );

				return false;
			}
		}

		for( TVectorPakMaterialDesc::iterator
			it = m_pathMaterials.begin(),
			it_end = m_pathMaterials.end();
		it != it_end;
		++it )
		{
			const PakMaterialDesc & desc = *it;

			if( this->loadMaterials_( m_name, desc.path ) == false )
			{
                LOGGER_ERROR( m_serviceProvider )("Package::enable '%s:%s' invalid load material '%s'"
                    , m_path.c_str()
                    , m_name.c_str()
					, desc.path.c_str()
                    );

				return false;
			}
		}

		m_enable = true;

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Package::validate()
	{
		bool successful = true;

		for( TVectorPakResourceDesc::const_iterator
			it = m_resourcesDesc.begin(),
			it_end = m_resourcesDesc.end();
		it != it_end;
		++it )
		{
			const PakResourceDesc & desc = *it;

			if( desc.ignored == true )
			{
				continue;
			}

			if( RESOURCE_SERVICE( m_serviceProvider )
				->validateResources( m_locale, m_name, desc.path ) == false )
			{
				successful = false;
			}
		}

		return successful;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Package::disable()
	{
		if( m_enable == false )
		{
			LOGGER_ERROR( m_serviceProvider )("Package::disable already disable '%s:%s' '%s'"
				, m_path.c_str()
				, m_name.c_str()
				, m_descriptionPath.c_str()
				);

			return false;
		}

		m_enable = false;

		SCRIPT_SERVICE( m_serviceProvider )
			->removeModulePath( m_name, m_scriptsPackages );

		for( TVectorPakResourceDesc::const_iterator
			it = m_resourcesDesc.begin(),
			it_end = m_resourcesDesc.end();
		it != it_end;
		++it )
		{
			const PakResourceDesc & desc = *it;

			if( RESOURCE_SERVICE( m_serviceProvider )
				->unloadResources( m_locale, m_name, desc.path ) == false )
			{
				return false;
			}
		}

		for( TVectorPakFontDesc::iterator
			it = m_pathFonts.begin(),
			it_end = m_pathFonts.end();
		it != it_end;
		++it )
		{
			const PakFontDesc & desc = *it;

			if( this->unloadFont_( m_name, desc.path ) == false )
			{
				return false;
			}
		}

		for( TVectorPakTextDesc::iterator
			it = m_pathTexts.begin(),
			it_end = m_pathTexts.end();
		it != it_end;
		++it )
		{
			const PakTextDesc & desc = *it;

			if( this->unloadText_( m_name, desc.path ) == false )
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

			if( this->removeUserData_( desc.name ) == false )
			{
				return false;
			}
		}

		for( TVectorPakMaterialDesc::iterator
			it = m_pathMaterials.begin(),
			it_end = m_pathMaterials.end();
		it != it_end;
		++it )
		{
			const PakMaterialDesc & desc = *it;

			if( this->unloadMaterials_( m_name, desc.path ) == false )
			{
				return false;
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Package::isEnable() const
	{
		return m_enable;
	}
    //////////////////////////////////////////////////////////////////////////
	bool Package::loadText_( const ConstString & _pakName, const ConstString & _path )
    {
        bool result = TEXT_SERVICE(m_serviceProvider)
			->loadTextEntry( _pakName, _path );

        return result;
    }
	//////////////////////////////////////////////////////////////////////////
	bool Package::unloadText_( const ConstString & _pakName, const ConstString & _path )
	{
		bool result = TEXT_SERVICE( m_serviceProvider )
			->unloadTextEntry( _pakName, _path );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Package::loadFont_( const ConstString & _pakName, const ConstString & _path )
	{
		bool result = TEXT_SERVICE(m_serviceProvider)
			->loadFonts( _pakName, _path );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Package::unloadFont_( const ConstString & _pakName, const ConstString & _path )
	{
		bool result = TEXT_SERVICE( m_serviceProvider )
			->unloadFonts( _pakName, _path );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Package::loadMaterials_( const ConstString & _pakName, const FilePath & _path )
	{
		bool result = RENDERMATERIAL_SERVICE( m_serviceProvider )
			->loadMaterials( _pakName, _path );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Package::unloadMaterials_( const ConstString & _pakName, const FilePath & _path )
	{
		bool result = RENDERMATERIAL_SERVICE( m_serviceProvider )
			->unloadMaterials( _pakName, _path );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Package::addUserData_( const ConstString & _pakName, const ConstString & _name, const FilePath & _path )
	{
		bool result = USERDATA_SERVICE(m_serviceProvider)
			->addUserdata( _name, _pakName, _path );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Package::removeUserData_( const ConstString & _name )
	{
		bool result = USERDATA_SERVICE( m_serviceProvider )
			->removeUserdata( _name );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void Package::addResource_( const FilePath & _path, const Tags & _platform, bool _ignored )
	{
		PakResourceDesc desc;
		desc.path = _path;
		desc.platform = _platform;
		desc.ignored = _ignored;

		m_resourcesDesc.push_back( desc );
	}
	//////////////////////////////////////////////////////////////////////////
	void Package::addTextPath_( const FilePath & _path, const Tags & _platform )
	{
		PakTextDesc desc;
		desc.path = _path;
		desc.platform = _platform;

		m_pathTexts.push_back( desc );
	}
	//////////////////////////////////////////////////////////////////////////
	void Package::addScriptPak_( const FilePath & _path, const ConstString & _module, const ConstString & _initializer, const ConstString & _finalizer, const Tags & _platform )
	{
		ScriptModulePack pak;
		pak.path = _path;
		pak.module = _module;
		pak.initializer = _initializer;
		pak.finalizer = _finalizer;
		pak.platform = _platform;

		m_scriptsPackages.push_back( pak );
	}
	//////////////////////////////////////////////////////////////////////////
	void Package::addFontPath_( const FilePath & _path, const Tags & _platform )
	{
		PakFontDesc desc;
		desc.path = _path;
		desc.platform = _platform;

		m_pathFonts.push_back( desc );
	}
	//////////////////////////////////////////////////////////////////////////
	void Package::addData_( const ConstString & _name, const FilePath & _path, const Tags & _platform )
	{
		PakDataDesc desc;
		desc.name = _name;
		desc.path = _path;
		desc.platform = _platform;

		m_datas.push_back( desc );
	}
	//////////////////////////////////////////////////////////////////////////
	void Package::addMaterial_( const FilePath & _path, const Tags & _platform )
	{ 
		PakMaterialDesc desc;
		desc.path = _path;
		desc.platform = _platform;

		m_pathMaterials.push_back( desc );
	}
}