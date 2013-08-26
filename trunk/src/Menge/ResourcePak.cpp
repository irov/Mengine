#	include "ResourcePak.h"

#	include "Interface/LoaderInterface.h"
#	include "Interface/FileSystemInterface.h"
#	include "Interface/ScriptSystemInterface.h"
#	include "Interface/TextInterface.h"
#	include "Interface/StringizeInterface.h"
#	include "Interface/ResourceInterface.h"
#	include "Interface/UnicodeInterface.h"
#	include "Interface/ProfilerInterface.h"

#	include "Metacode.h"

#   include "Logger/Logger.h"

#	include "Core/File.h"

#	include "Consts.h"

#   include "pugixml.hpp"

#   include <stdio.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ResourcePak::ResourcePak( ServiceProviderInterface * _serviceProvider, const ConstString & _name, const ConstString & _type, const ConstString & _locale, const ConstString & _platform, const FilePath & _filename, const FilePath & _path, bool _preload, const FilePath & _baseDir )
		: m_serviceProvider(_serviceProvider)
        , m_name(_name)
		, m_type(_type)
		, m_locale(_locale)
		, m_platform(_platform)
		, m_filename(_filename)
		, m_path(_path)
		, m_preload(_preload)
		, m_baseDir(_baseDir)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourcePak::isPreload() const
	{
		return m_preload;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourcePak::getName() const
	{
		return m_name;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourcePak::getLocale() const
	{
		return m_locale;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourcePak::getPlatfrom() const
	{
		return m_platform;
	}
	//////////////////////////////////////////////////////////////////////////
	const FilePath & ResourcePak::getPath() const
	{
		return m_path;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourcePak::load()
	{		
		if( FILE_SERVICE(m_serviceProvider)->mountFileGroup( m_name, m_baseDir, m_path, m_type, false ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)( "ResourcePak::load failed to mount pak '%s' path '%s':'%s'"
				, m_name.c_str()
				, m_baseDir.c_str()
                , m_path.c_str()
				);

			return false;
		}

		Metacode::Meta_Pak pak;

		bool exist = false;
		if( LOADER_SERVICE(m_serviceProvider)->load( m_name, m_filename, &pak, exist ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)( "ResourcePak::load Invalid resource file '%s:%s' '%s'"
				, m_path.c_str()
				, m_name.c_str()
				, m_filename.c_str()
				);

			return false;
		}

        m_resourcesDesc.reserve( 32 );
        m_textsDesc.reserve( 8 );

		const Metacode::Meta_Pak::TVectorMeta_Scripts & includes_scripts = pak.get_IncludesScripts();

		for( Metacode::Meta_Pak::TVectorMeta_Scripts::const_iterator
			it = includes_scripts.begin(),
			it_end = includes_scripts.end();
		it != it_end;
		++it )
		{
			const Metacode::Meta_Pak::Meta_Scripts & scripts = *it;

			scripts.method_Path( this, &ResourcePak::addScriptPath_ );
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

				meta_resource.method_Path( this, &ResourcePak::addResource_ );
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

                meta_text.method_Path( this, &ResourcePak::addText_ );
			}
		}

		SCRIPT_SERVICE(m_serviceProvider)
            ->addModulePath( m_name, m_pathScripts );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourcePak::apply()
	{
		for( TResourceDescs::const_iterator
			it = m_resourcesDesc.begin(),
			it_end = m_resourcesDesc.end();
		it != it_end;
		++it )
		{
            //PROFILER_SERVICE(m_serviceProvider)
            //    ->memoryBegin();

            const ResourceDesc & desc = *it;

			if( RESOURCE_SERVICE(m_serviceProvider)->loadResource( desc.pakName, desc.path ) == false )
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

		for( TTextDescs::iterator
			it = m_textsDesc.begin(),
			it_end = m_textsDesc.end();
		it != it_end;
		++it )
		{
            const ConstString & pakName = it->pakName;
            const FilePath & path = it->path;

			if( this->loadText_( pakName, path ) == false )
            {
                return false;
            }
		}

        return true;
	}
    //////////////////////////////////////////////////////////////////////////
    bool ResourcePak::loadText_( const ConstString & _pakName, const FilePath & _path )
    {
        bool result = TEXT_SERVICE(m_serviceProvider)
			->loadTextEntry( m_locale, _pakName, _path );

        return result;
    }
	//////////////////////////////////////////////////////////////////////////
	void ResourcePak::addResource_( const FilePath & _path )
	{
		ResourceDesc desc;
		desc.pakName = m_name;
		desc.pakPath = m_path;
		desc.pakType = m_type;
		desc.path = _path;

		m_resourcesDesc.push_back( desc );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePak::addText_( const FilePath & _path )
	{
		ResourceDesc desc;
		desc.pakName = m_name;
		desc.pakPath = m_path;
		desc.pakType = m_type;
		desc.path = _path;

		m_textsDesc.push_back( desc );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePak::addScriptPath_( const FilePath & _path )
	{
		m_pathScripts.push_back( _path );
	}
}