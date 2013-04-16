#	include "ResourcePak.h"

#	include "Interface/LoaderInterface.h"
#	include "Interface/FileSystemInterface.h"
#	include "Interface/ScriptSystemInterface.h"
#	include "Interface/TextInterface.h"
#	include "Interface/StringizeInterface.h"
#	include "Interface/ResourceInterface.h"
#	include "Interface/UnicodeInterface.h"

#	include "Interface/SceneInterface.h"
#	include "Interface/ArrowInterface.h"
#	include "Interface/EntityInterface.h"

#	include "Metacode.h"

#   include "Logger/Logger.h"

#	include "Core/File.h"

#	include "Consts.h"

#   include "pugixml.hpp"

#   include <stdio.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ResourcePak::ResourcePak( ServiceProviderInterface * _serviceProvider, const ConstString & _name, const ConstString & _type, const ConstString & _locale, const String & _platform, const String & _filename, const String & _path, bool _preload, const String & _baseDir )
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
	const String & ResourcePak::getPlatfrom() const
	{
		return m_platform;
	}
	//////////////////////////////////////////////////////////////////////////
	const String & ResourcePak::getPath() const
	{
		return m_path;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourcePak::load()
	{		
		static String cache_fullpath;

        cache_fullpath = m_baseDir;
		cache_fullpath += m_path;
		
		if(  m_type == CONST_STRING(m_serviceProvider, zip) )
		{
			//fullPakPath += L".zip";
		}
		else
		{
			cache_fullpath += MENGE_FOLDER_DELIM;
		}

        FilePath fullpath = Helper::stringizeString( m_serviceProvider, cache_fullpath );

		if( FILE_SERVICE(m_serviceProvider)->mountFileSystem( m_name, fullpath, m_type, false ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)( "ResourcePak::load failed to mount pak '%s:%s'"
				, m_name.c_str()
				, fullpath.c_str()
				);

			return false;
		}

		Metacode::Meta_Pak pak;

        FilePath filepath = Helper::stringizeString( m_serviceProvider, m_filename );

		bool exist = false;
		if( LOADER_SERVICE(m_serviceProvider)->load( m_name, filepath, &pak, exist ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)( "ResourcePak::load Invalid resource file '%s:%s' '%s'"
				, m_path.c_str()
				, m_name.c_str()
				, m_filename.c_str()
				);

			return false;
		}

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
		for( TResourceDescs::iterator
			it = m_resourcesDesc.begin(),
			it_end = m_resourcesDesc.end();
		it != it_end;
		++it )
		{
			if( RESOURCE_SERVICE(m_serviceProvider)->loadResource( *it ) == false )
            {
                return false;
            }
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
        InputStreamInterface * xml_text = FILE_SERVICE(m_serviceProvider)
            ->openInputFile( _pakName, _path );

        if( xml_text == NULL )
        {
            LOGGER_ERROR(m_serviceProvider)("TextManager::loadResource invalid open file %s:%s"
                , _pakName.c_str()
                , _path.c_str()
                );

            return false;
        }

        size_t xml_buffer_size = xml_text->size();
        char * xml_buffer = new char[xml_buffer_size];

        xml_text->read( xml_buffer, xml_buffer_size );

        xml_text->destroy();
        xml_text = NULL;


        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load_buffer( xml_buffer, xml_buffer_size );

        delete [] xml_buffer;

        if( result == false )
        {
            LOGGER_ERROR(m_serviceProvider)("TextManager::loadResource invalid format file %s:%s xml error %s"
                , _pakName.c_str()
                , _path.c_str()
                , result.description()
                );

            return false;
        }

        pugi::xml_node root = doc.document_element();

        const char * root_name = root.name();

        if( strcmp( root_name, "Texts" ) != 0 )
        {
            LOGGER_ERROR(m_serviceProvider)("TextManager::loadResource invalid format file %s:%s root node not Texts"
                , _pakName.c_str()
                , _path.c_str()
                );

            return false;
        }

        for( pugi::xml_node::iterator
            it = root.begin(),
            it_end = root.end();
        it != it_end;
        ++it )
        {
            const pugi::xml_node & element = *it;

            const char * element_name = element.name();

            if( strcmp( element_name, "Text" ) != 0 )
            {
                continue;
            }

            pugi::xml_attribute Key = element.attribute("Key");

            if( Key.empty() == true )
            {
                continue;
            }

            const char * attribute_key = Key.value();

            TextEntry textEntry;

            textEntry.charOffset = 0.f;
            textEntry.lineOffset = 0.f;

            pugi::xml_attribute Value = element.attribute("Value");

            if( Value.empty() == false )
            {
                const char * value = Value.value();

                String utf8_value(value);

                if( Helper::utf8ToUnicode( m_serviceProvider, utf8_value, textEntry.text ) == false )
                {
                    LOGGER_WARNING(m_serviceProvider)("TextManager::loadResource %s:%s invalid conver %s:%s from utf8 to unicode"
                        , _pakName.c_str()
                        , _path.c_str()
                        , attribute_key
                        , value
                        );
                }
            }

            pugi::xml_attribute Font = element.attribute("Font");

            if( Font.empty() == false )
            {
                const char * value = Font.value();

                textEntry.font = Helper::stringizeString(m_serviceProvider, value);
            }

            pugi::xml_attribute CharOffset = element.attribute("CharOffset");

            if( CharOffset.empty() == false )
            {
                const char * value = CharOffset.value();

                if( sscanf( value, "%f", &textEntry.charOffset ) != 1 )
                {
                    LOGGER_WARNING(m_serviceProvider)("TextManager::loadResource %s:%s invalid read for text %s charOffset %s"
                        , _pakName.c_str()
                        , _path.c_str()
                        , attribute_key
                        , value
                        );
                }
            }

            pugi::xml_attribute LineOffset = element.attribute("LineOffset");

            if( LineOffset.empty() == false )
            {
                const char * value = LineOffset.value();

                if( sscanf( value, "%f", &textEntry.lineOffset ) != 1 )
                {
                    LOGGER_WARNING(m_serviceProvider)("TextManager::loadResource %s:%s invalid read for text %s lineOffset %s"
                        , _pakName.c_str()
                        , _path.c_str()
                        , attribute_key
                        , value
                        );
                }
            }

            TEXT_SERVICE(m_serviceProvider)
                ->addTextEntry( Helper::stringizeString(m_serviceProvider, attribute_key), textEntry );
        }

        return true;
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