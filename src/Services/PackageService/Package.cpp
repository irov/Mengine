#include "Package.h"

#include "Interface/LoaderServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/TextServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/UnicodeSystemInterface.h"
#include "Interface/UserdataServiceInterface.h"
#include "Interface/PlatformInterface.h"
#include "Interface/RenderMaterialServiceInterface.h"

#include "Metacode/Metacode.h"

#include "Kernel/Logger.h"

#include <stdio.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Package::Package()
        : m_preload( false )
        , m_load( false )
        , m_enable( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Package::~Package()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Package::initialize( const ConstString & _name
        , const ConstString & _type
        , const ConstString & _locale
        , const Tags & _platform
        , const FilePath & _descriptionPath
        , const FileGroupInterfacePtr & _mountFileGroup
        , const FilePath & _path
        , bool _preload )
    {
        m_name = _name;
        m_type = _type;
        m_locale = _locale;
        m_platform = _platform;
        m_descriptionPath = _descriptionPath;
        m_mountFileGroup = _mountFileGroup;
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
    void Package::setName( const ConstString & _name )
    {
        m_name = _name;
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
    void Package::setPath( const FilePath & _path )
    {
        m_path = _path;
    }
    //////////////////////////////////////////////////////////////////////////
    const FilePath & Package::getPath() const
    {
        return m_path;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Package::load( const Char * _doc )
    {
        if( this->mountFileGroup_( _doc ) == false )
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
    bool Package::mountFileGroup_( const Char * _doc )
    {
        FileGroupInterfacePtr fileGroup;
        if( FILE_SERVICE()
            ->mountFileGroup( m_name, m_mountFileGroup, m_path, m_type, &fileGroup, _doc ) == false )
        {
            LOGGER_ERROR( "failed to mount pak '%s' path '%s'"
                , m_name.c_str()
                , m_path.c_str()
            );

            return false;
        }

        m_fileGroup = fileGroup;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Package::unmountFileGroup_()
    {
        if( FILE_SERVICE()
            ->unmountFileGroup( m_name ) == false )
        {
            LOGGER_ERROR( "failed to mount pak '%s' path '%s'"
                , m_name.c_str()
                , m_path.c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Package::loadPak_()
    {
        if( SERVICE_EXIST( LoaderServiceInterface ) == false )
        {
            return false;
        }

        if( m_descriptionPath.empty() == true )
        {
            return true;
        }

        Metacode::Meta_Data::Meta_Pak pak;

        bool exist = false;
        if( LOADER_SERVICE()
            ->load( m_fileGroup, m_descriptionPath, &pak, Metacode::Meta_Data::getVersion(), exist ) == false )
        {
            LOGGER_ERROR( "invalid resource file '%s:%s' '%s'"
                , m_path.c_str()
                , m_name.c_str()
                , m_descriptionPath.c_str()
            );

            return false;
        }

        m_resourcesDesc.reserve( 32 );

        const Metacode::Meta_Data::Meta_Pak::VectorMeta_Scripts & includes_scripts = pak.get_Includes_Scripts();

        for( Metacode::Meta_Data::Meta_Pak::VectorMeta_Scripts::const_iterator
            it = includes_scripts.begin(),
            it_end = includes_scripts.end();
            it != it_end;
            ++it )
        {
            const Metacode::Meta_Data::Meta_Pak::Meta_Scripts & meta_scripts = *it;

            const FilePath & Path = meta_scripts.get_Path();

            ConstString Module;
            meta_scripts.get_Module( &Module );
            ConstString Initializer;
            meta_scripts.get_Initializer( &Initializer );

            ConstString Finalizer;
            meta_scripts.get_Finalizer( &Finalizer );

            Tags Platform;
            meta_scripts.get_Platform( &Platform );

            this->addScriptPak_( Path, Module, Initializer, Finalizer, Platform );
        }

        const Metacode::Meta_Data::Meta_Pak::VectorMeta_Fonts & includes_fonts = pak.get_Includes_Fonts();

        for( Metacode::Meta_Data::Meta_Pak::VectorMeta_Fonts::const_iterator
            it = includes_fonts.begin(),
            it_end = includes_fonts.end();
            it != it_end;
            ++it )
        {
            const Metacode::Meta_Data::Meta_Pak::Meta_Fonts & meta_fonts = *it;

            const FilePath & Path = meta_fonts.get_Path();

            Tags Platform;
            meta_fonts.get_Platform( &Platform );

            this->addFontPath_( Path, Platform );
        }

        const Metacode::Meta_Data::Meta_Pak::VectorMeta_Resources & includes_resources = pak.get_Includes_Resources();

        for( Metacode::Meta_Data::Meta_Pak::VectorMeta_Resources::const_iterator
            it = includes_resources.begin(),
            it_end = includes_resources.end();
            it != it_end;
            ++it )
        {
            const Metacode::Meta_Data::Meta_Pak::Meta_Resources & meta_resources = *it;

            bool ignored = false;
            meta_resources.get_Ignored( &ignored );

            bool demand = false;
            meta_resources.get_Demand( &demand );

            Tags platform;
            meta_resources.get_Platform( &platform );

            const Metacode::Meta_Data::Meta_Pak::Meta_Resources::VectorMeta_Resource & includes_resource = meta_resources.get_Includes_Resource();

            for( Metacode::Meta_Data::Meta_Pak::Meta_Resources::VectorMeta_Resource::const_iterator
                it_include = includes_resource.begin(),
                it_include_end = includes_resource.end();
                it_include != it_include_end;
                ++it_include )
            {
                const Metacode::Meta_Data::Meta_Pak::Meta_Resources::Meta_Resource & meta_resource = *it_include;

                const FilePath & Path = meta_resource.get_Path();

                Tags tags;
                meta_resource.get_Tags( &tags );

                this->addResource_( Path, tags, platform, demand, ignored );
            }
        }

        const Metacode::Meta_Data::Meta_Pak::VectorMeta_Texts & includes_tests = pak.get_Includes_Texts();

        for( Metacode::Meta_Data::Meta_Pak::VectorMeta_Texts::const_iterator
            it = includes_tests.begin(),
            it_end = includes_tests.end();
            it != it_end;
            ++it )
        {
            const Metacode::Meta_Data::Meta_Pak::Meta_Texts & meta_texts = *it;

            Tags platform;
            meta_texts.get_Platform( &platform );

            const Metacode::Meta_Data::Meta_Pak::Meta_Texts::VectorMeta_Text & includes_text = meta_texts.get_Includes_Text();

            for( Metacode::Meta_Data::Meta_Pak::Meta_Texts::VectorMeta_Text::const_iterator
                it_include = includes_text.begin(),
                it_include_end = includes_text.end();
                it_include != it_include_end;
                ++it_include )
            {
                const Metacode::Meta_Data::Meta_Pak::Meta_Texts::Meta_Text & meta_text = *it_include;

                const FilePath & Path = meta_text.get_Path();
                this->addTextPath_( Path, platform );
            }
        }

        const Metacode::Meta_Data::Meta_Pak::VectorMeta_Datas & includes_datas = pak.get_Includes_Datas();

        for( Metacode::Meta_Data::Meta_Pak::VectorMeta_Datas::const_iterator
            it = includes_datas.begin(),
            it_end = includes_datas.end();
            it != it_end;
            ++it )
        {
            const Metacode::Meta_Data::Meta_Pak::Meta_Datas & meta_datas = *it;

            Tags platform;
            meta_datas.get_Platform( &platform );

            const Metacode::Meta_Data::Meta_Pak::Meta_Datas::VectorMeta_Data & includes_data = meta_datas.get_Includes_Data();

            for( Metacode::Meta_Data::Meta_Pak::Meta_Datas::VectorMeta_Data::const_iterator
                it_include = includes_data.begin(),
                it_include_end = includes_data.end();
                it_include != it_include_end;
                ++it_include )
            {
                const Metacode::Meta_Data::Meta_Pak::Meta_Datas::Meta_Data & meta_data = *it_include;

                const ConstString & name = meta_data.get_Name();
                const FilePath & path = meta_data.get_Path();

                this->addData_( name, path, platform );
            }
        }

        const Metacode::Meta_Data::Meta_Pak::VectorMeta_Materials & includes_materials = pak.get_Includes_Materials();

        for( Metacode::Meta_Data::Meta_Pak::VectorMeta_Materials::const_iterator
            it = includes_materials.begin(),
            it_end = includes_materials.end();
            it != it_end;
            ++it )
        {
            const Metacode::Meta_Data::Meta_Pak::Meta_Materials & meta_materials = *it;

            Tags platform;
            meta_materials.get_Platform( &platform );

            const Metacode::Meta_Data::Meta_Pak::Meta_Materials::VectorMeta_Material & includes_material = meta_materials.get_Includes_Material();

            for( Metacode::Meta_Data::Meta_Pak::Meta_Materials::VectorMeta_Material::const_iterator
                it_include = includes_material.begin(),
                it_include_end = includes_material.end();
                it_include != it_include_end;
                ++it_include )
            {
                const Metacode::Meta_Data::Meta_Pak::Meta_Materials::Meta_Material & meta_material = *it_include;

                const FilePath & path = meta_material.get_Path();

                this->addMaterial_( path, platform );
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Package::enable()
    {
        LOGGER_MESSAGE( "Package enable... '%s'"
            , m_name.c_str()
        );

        if( m_enable == true )
        {
            LOGGER_ERROR( "already enable '%s:%s' '%s'"
                , m_path.c_str()
                , m_name.c_str()
                , m_descriptionPath.c_str()
            );

            return false;
        }

        if( SERVICE_EXIST( ScriptServiceInterface ) == true )
        {
            SCRIPT_SERVICE()
                ->addModulePath( m_fileGroup, m_scriptsPackages );
        }

        const Tags & platformTags = PLATFORM_SERVICE()
            ->getPlatformTags();

        for( const PakResourceDesc & desc : m_resourcesDesc )
        {
            if( desc.platform.empty() == false && platformTags.hasTags( desc.platform ) == false )
            {
                continue;
            }

            if( RESOURCE_SERVICE()
                ->loadResources( m_locale, m_fileGroup, desc.path, desc.tags, desc.ignored ) == false )
            {
                LOGGER_ERROR( "invalid load '%s:%s' resource '%s'"
                    , m_path.c_str()
                    , m_name.c_str()
                    , desc.path.c_str()
                );

                return false;
            }
        }

        for( const PakFontDesc & desc : m_pathFonts )
        {
            if( desc.platform.empty() == false && desc.platform.hasTags( platformTags ) == false )
            {
                continue;
            }

            if( this->loadFont_( desc.path ) == false )
            {
                LOGGER_ERROR( "Package::enable '%s:%s' invalid load font '%s'"
                    , m_path.c_str()
                    , m_name.c_str()
                    , desc.path.c_str()
                );

                return false;
            }
        }

        for( const PakTextDesc & desc : m_pathTexts )
        {
            if( desc.platform.empty() == false && desc.platform.hasTags( platformTags ) == false )
            {
                continue;
            }

            if( this->loadText_( desc.path ) == false )
            {
                LOGGER_ERROR( "Package::enable '%s:%s' invalid load text '%s'"
                    , m_path.c_str()
                    , m_name.c_str()
                    , desc.path.c_str()
                );

                return false;
            }
        }

        for( const PakDataDesc & desc : m_datas )
        {
            if( desc.platform.empty() == false && desc.platform.hasTags( platformTags ) == false )
            {
                continue;
            }

            if( this->addUserData_( desc.name, desc.path ) == false )
            {
                LOGGER_ERROR( "Package::enable '%s:%s' invalid load userdata '%s' path '%s'"
                    , m_path.c_str()
                    , m_name.c_str()
                    , desc.name.c_str()
                    , desc.path.c_str()
                );

                return false;
            }
        }

        for( const PakMaterialDesc & desc : m_pathMaterials )
        {
            if( desc.platform.empty() == false && desc.platform.hasTags( platformTags ) == false )
            {
                continue;
            }

            if( this->loadMaterials_( desc.path ) == false )
            {
                LOGGER_ERROR( "Package::enable '%s:%s' invalid load material '%s'"
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
    bool Package::disable()
    {
        if( m_enable == false )
        {
            LOGGER_ERROR( "Package::disable already disable '%s:%s' '%s'"
                , m_path.c_str()
                , m_name.c_str()
                , m_descriptionPath.c_str()
            );

            return false;
        }

        m_enable = false;

        if( SERVICE_EXIST( ScriptServiceInterface ) == true )
        {
            SCRIPT_SERVICE()
                ->removeModulePath( m_fileGroup, m_scriptsPackages );
        }

        const Tags & platformTags = PLATFORM_SERVICE()
            ->getPlatformTags();

        for( const PakResourceDesc & desc : m_resourcesDesc )
        {
            if( desc.platform.empty() == false && desc.platform.hasTags( platformTags ) == false )
            {
                continue;
            }

            if( RESOURCE_SERVICE()
                ->unloadResources( m_locale, m_fileGroup, desc.path ) == false )
            {
                return false;
            }
        }

        for( const PakFontDesc & desc : m_pathFonts )
        {
            if( desc.platform.empty() == false && desc.platform.hasTags( platformTags ) == false )
            {
                continue;
            }

            if( this->unloadFont_( desc.path ) == false )
            {
                return false;
            }
        }

        for( const PakTextDesc & desc : m_pathTexts )
        {
            if( desc.platform.empty() == false && desc.platform.hasTags( platformTags ) == false )
            {
                continue;
            }

            if( this->unloadText_( desc.path ) == false )
            {
                return false;
            }
        }

        for( const PakDataDesc & desc : m_datas )
        {
            if( desc.platform.empty() == false && desc.platform.hasTags( platformTags ) == false )
            {
                continue;
            }

            if( this->removeUserData_( desc.name ) == false )
            {
                return false;
            }
        }

        for( const PakMaterialDesc & desc : m_pathMaterials )
        {
            if( desc.platform.empty() == false && desc.platform.hasTags( platformTags ) == false )
            {
                continue;
            }

            if( this->unloadMaterials_( desc.path ) == false )
            {
                return false;
            }
        }

        if( this->unmountFileGroup_() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Package::isEnable() const
    {
        return m_enable;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Package::loadText_( const FilePath & _path )
    {
        bool result = TEXT_SERVICE()
            ->loadTextEntry( m_fileGroup, _path );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Package::unloadText_( const FilePath & _path )
    {
        bool result = TEXT_SERVICE()
            ->unloadTextEntry( m_fileGroup, _path );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Package::loadFont_( const FilePath & _path )
    {
        bool result = TEXT_SERVICE()
            ->loadFonts( m_fileGroup, _path );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Package::unloadFont_( const FilePath & _path )
    {
        bool result = TEXT_SERVICE()
            ->unloadFonts( m_fileGroup, _path );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Package::loadMaterials_( const FilePath & _path )
    {
        bool result = RENDERMATERIAL_SERVICE()
            ->loadMaterials( m_fileGroup, _path );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Package::unloadMaterials_( const FilePath & _path )
    {
        bool result = RENDERMATERIAL_SERVICE()
            ->unloadMaterials( m_fileGroup, _path );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Package::addUserData_( const ConstString & _name, const FilePath & _path )
    {
        bool result = USERDATA_SERVICE()
            ->addUserdata( _name, m_fileGroup, _path );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Package::removeUserData_( const ConstString & _name )
    {
        bool result = USERDATA_SERVICE()
            ->removeUserdata( _name );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    void Package::addResource_( const FilePath & _path, const Tags & _tags, const Tags & _platform, bool _demand, bool _ignored )
    {
        PakResourceDesc desc;
        desc.path = _path;
        desc.tags = _tags;
        desc.platform = _platform;
        desc.demand = _demand;
        desc.ignored = _ignored;

        m_resourcesDesc.emplace_back( desc );
    }
    //////////////////////////////////////////////////////////////////////////
    void Package::addTextPath_( const FilePath & _path, const Tags & _platform )
    {
        PakTextDesc desc;
        desc.path = _path;
        desc.platform = _platform;

        m_pathTexts.emplace_back( desc );
    }
    //////////////////////////////////////////////////////////////////////////
    void Package::addScriptPak_( const FilePath & _path, const ConstString & _module, const ConstString & _initializer, const ConstString & _finalizer, const Tags & _platform )
    {
        ScriptModulePack pak;
        pak.fileGroup = m_fileGroup;
        pak.path = _path;
        pak.module = _module;
        pak.initializer = _initializer;
        pak.finalizer = _finalizer;
        pak.platform = _platform;

        m_scriptsPackages.emplace_back( pak );
    }
    //////////////////////////////////////////////////////////////////////////
    void Package::addFontPath_( const FilePath & _path, const Tags & _platform )
    {
        PakFontDesc desc;
        desc.path = _path;
        desc.platform = _platform;

        m_pathFonts.emplace_back( desc );
    }
    //////////////////////////////////////////////////////////////////////////
    void Package::addData_( const ConstString & _name, const FilePath & _path, const Tags & _platform )
    {
        PakDataDesc desc;
        desc.name = _name;
        desc.path = _path;
        desc.platform = _platform;

        m_datas.emplace_back( desc );
    }
    //////////////////////////////////////////////////////////////////////////
    void Package::addMaterial_( const FilePath & _path, const Tags & _platform )
    {
        PakMaterialDesc desc;
        desc.path = _path;
        desc.platform = _platform;

        m_pathMaterials.emplace_back( desc );
    }
}