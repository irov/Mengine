#include "Package.h"

#include "Interface/LoaderServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/TextServiceInterface.h"
#include "Interface/FontServiceInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/UnicodeSystemInterface.h"
#include "Interface/UserdataServiceInterface.h"
#include "Interface/RenderSystemInterface.h"
#include "Interface/PlatformInterface.h"
#include "Interface/RenderMaterialServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"
#include "Interface/NotificationServiceInterface.h"
#include "Interface/SettingsServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"

#include "Metacode/Metacode.h"

#include "Kernel/Logger.h"
#include "Kernel/Resource.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/FileGroupHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Package::Package()
        : m_load( false )
        , m_enable( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Package::~Package()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Package::initialize( const FileGroupInterfacePtr & _baseFileGroup, const PackageDesc & _desc )
    {
        m_baseFileGroup = _baseFileGroup;
        m_desc = _desc;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Package::finalize()
    {
        this->unmountFileGroup_();

        m_resourcesDesc.clear();

        m_parentPackage = nullptr;
        m_baseFileGroup = nullptr;

        m_scriptsPackages.clear();
        m_fontsDesc.clear();
        m_textsDesc.clear();
        m_datasDesc.clear();
        m_materialsDesc.clear();
        m_settingsDesc.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    const PackageDesc & Package::getPackageDesc() const
    {
        return m_desc;
    }
    //////////////////////////////////////////////////////////////////////////
    void Package::setParent( const PackageInterfacePtr & _package )
    {
        m_parentPackage = _package;
    }
    //////////////////////////////////////////////////////////////////////////
    const PackageInterfacePtr & Package::getParent() const
    {
        return m_parentPackage;
    }
    //////////////////////////////////////////////////////////////////////////
    const FileGroupInterfacePtr & Package::getFileGroup() const
    {
        return m_fileGroup;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Package::load( const DocumentPtr & _doc )
    {
        if( this->mountFileGroup_( _doc ) == false )
        {
            return false;
        }

        if( this->loadPackage_() == false )
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
    bool Package::mountFileGroup_( const DocumentPtr & _doc )
    {
        const PackageInterfacePtr & parentPackage = this->getParent();
        const FileGroupInterfacePtr & parentFileGroup = (parentPackage == nullptr) ? FileGroupInterfacePtr::none() : parentPackage->getFileGroup();

        FileGroupInterfacePtr fileGroup;
        if( FILE_SERVICE()
            ->mountFileGroup( m_desc.name, m_baseFileGroup, parentFileGroup, m_desc.path, m_desc.type, &fileGroup, false, _doc ) == false )
        {
            LOGGER_ERROR( "failed to mount package '%s' path '%s'"
                , m_desc.name.c_str()
                , m_desc.path.c_str()
            );

            return false;
        }

        m_fileGroup = fileGroup;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Package::unmountFileGroup_()
    {
        FILE_SERVICE()
            ->unmountFileGroup( m_desc.name );

        m_fileGroup = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Package::loadPackage_()
    {
        if( m_desc.fontsPath.empty() == false )
        {
            this->addFontPath_( m_desc.fontsPath, Tags() );
        }

        if( m_desc.textsPath.empty() == false )
        {
            this->addTextPath_( m_desc.textsPath, Tags() );
        }

        if( SERVICE_EXIST( LoaderServiceInterface ) == false )
        {
            return false;
        }

        if( m_desc.descriptionPath.empty() == true )
        {
            return true;
        }

        Metacode::Meta_Data::Meta_Pak pak;

        bool exist = false;
        if( LOADER_SERVICE()
            ->load( m_fileGroup, m_desc.descriptionPath, &pak, Metacode::Meta_Data::getVersion(), &exist ) == false )
        {
            LOGGER_ERROR( "invalid resource file '%s' name '%s' description '%s'"
                , m_desc.path.c_str()
                , m_desc.name.c_str()
                , m_desc.descriptionPath.c_str()
            );

            return false;
        }

        m_resourcesDesc.reserve( 32 );

        const Metacode::Meta_Data::Meta_Pak::VectorMeta_Scripts & includes_scripts = pak.get_Includes_Scripts();

        for( const Metacode::Meta_Data::Meta_Pak::Meta_Scripts & meta_scripts : includes_scripts )
        {
            Tags Platform;
            meta_scripts.get_Platform( &Platform );

            const Metacode::Meta_Data::Meta_Pak::Meta_Scripts::VectorMeta_Script & includes_script = meta_scripts.get_Includes_Script();

            for( const Metacode::Meta_Data::Meta_Pak::Meta_Scripts::Meta_Script & meta_script : includes_script )
            {
                const FilePath & Path = meta_script.get_Path();

                ConstString Module;
                meta_script.get_Module( &Module );

                ConstString Initializer;
                meta_script.get_Initializer( &Initializer );

                ConstString Finalizer;
                meta_script.get_Finalizer( &Finalizer );


                this->addScriptPackage_( Path, Module, Initializer, Finalizer, Platform );
            }
        }

        const Metacode::Meta_Data::Meta_Pak::VectorMeta_Fonts & includes_fonts = pak.get_Includes_Fonts();

        for( const Metacode::Meta_Data::Meta_Pak::Meta_Fonts & meta_fonts : includes_fonts )
        {
            Tags Platform;
            meta_fonts.get_Platform( &Platform );

            const Metacode::Meta_Data::Meta_Pak::Meta_Fonts::VectorMeta_Font & includes_font = meta_fonts.get_Includes_Font();

            for( const Metacode::Meta_Data::Meta_Pak::Meta_Fonts::Meta_Font & meta_font : includes_font )
            {
                const FilePath & Path = meta_font.get_Path();

                this->addFontPath_( Path, Platform );
            }
        }

        const Metacode::Meta_Data::Meta_Pak::VectorMeta_Resources & includes_resources = pak.get_Includes_Resources();

        for( const Metacode::Meta_Data::Meta_Pak::Meta_Resources & meta_resources : includes_resources )
        {
            bool ignored = false;
            meta_resources.get_Ignored( &ignored );

            bool demand = false;
            meta_resources.get_Demand( &demand );

            Tags platform;
            meta_resources.get_Platform( &platform );

            const Metacode::Meta_Data::Meta_Pak::Meta_Resources::VectorMeta_Resource & includes_resource = meta_resources.get_Includes_Resource();

            for( const Metacode::Meta_Data::Meta_Pak::Meta_Resources::Meta_Resource & meta_resource : includes_resource )
            {
                const FilePath & Path = meta_resource.get_Path();

                Tags tags;
                meta_resource.get_Tags( &tags );

                this->addResource_( Path, tags, platform, demand, ignored );
            }
        }

        const Metacode::Meta_Data::Meta_Pak::VectorMeta_Texts & includes_tests = pak.get_Includes_Texts();

        for( const Metacode::Meta_Data::Meta_Pak::Meta_Texts & meta_texts : includes_tests )
        {
            Tags platform;
            meta_texts.get_Platform( &platform );

            const Metacode::Meta_Data::Meta_Pak::Meta_Texts::VectorMeta_Text & includes_text = meta_texts.get_Includes_Text();

            for( const Metacode::Meta_Data::Meta_Pak::Meta_Texts::Meta_Text & meta_text : includes_text )
            {
                const FilePath & Path = meta_text.get_Path();
                this->addTextPath_( Path, platform );
            }
        }

        const Metacode::Meta_Data::Meta_Pak::VectorMeta_Datas & includes_datas = pak.get_Includes_Datas();

        for( const Metacode::Meta_Data::Meta_Pak::Meta_Datas & meta_datas : includes_datas )
        {
            Tags platform;
            meta_datas.get_Platform( &platform );

            const Metacode::Meta_Data::Meta_Pak::Meta_Datas::VectorMeta_Data & includes_data = meta_datas.get_Includes_Data();

            for( const Metacode::Meta_Data::Meta_Pak::Meta_Datas::Meta_Data & meta_data : includes_data )
            {
                const ConstString & name = meta_data.get_Name();
                const FilePath & path = meta_data.get_Path();

                this->addData_( name, path, platform );
            }
        }

        const Metacode::Meta_Data::Meta_Pak::VectorMeta_Materials & includes_materials = pak.get_Includes_Materials();

        for( const Metacode::Meta_Data::Meta_Pak::Meta_Materials & meta_materials : includes_materials )
        {
            Tags platform;
            meta_materials.get_Platform( &platform );

            const Metacode::Meta_Data::Meta_Pak::Meta_Materials::VectorMeta_Material & includes_material = meta_materials.get_Includes_Material();

            for( const Metacode::Meta_Data::Meta_Pak::Meta_Materials::Meta_Material & meta_material : includes_material )
            {
                const FilePath & path = meta_material.get_Path();

                this->addMaterial_( path, platform );
            }
        }

        const Metacode::Meta_Data::Meta_Pak::VectorMeta_Settings & includes_settings = pak.get_Includes_Settings();

        for( const Metacode::Meta_Data::Meta_Pak::Meta_Settings & meta_settings : includes_settings )
        {
            Tags platform;
            meta_settings.get_Platform( &platform );

            const Metacode::Meta_Data::Meta_Pak::Meta_Settings::VectorMeta_Setting & includes_setting = meta_settings.get_Includes_Setting();

            for( const Metacode::Meta_Data::Meta_Pak::Meta_Settings::Meta_Setting & meta_setting : includes_setting )
            {
                const ConstString & name = meta_setting.get_Name();
                const FilePath & path = meta_setting.get_Path();

                this->addSetting_( name, path, platform );
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Package::enable()
    {
        LOGGER_MESSAGE( "Package enable... '%s'"
            , m_desc.name.c_str()
        );

        if( m_enable == true )
        {
            LOGGER_ERROR( "already enable file '%s' name '%s' description '%s'"
                , m_desc.path.c_str()
                , m_desc.name.c_str()
                , m_desc.descriptionPath.c_str()
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

        for( const PackageResourceDesc & desc : m_resourcesDesc )
        {
            if( desc.platform.empty() == false && platformTags.hasTags( desc.platform ) == false )
            {
                continue;
            }

            if( this->loadResources_( m_desc.locale, m_fileGroup, desc.path, desc.tags, desc.ignored ) == false )
            {
                LOGGER_ERROR( "invalid load file '%s' name '%s' resource '%s'"
                    , m_desc.path.c_str()
                    , m_desc.name.c_str()
                    , desc.path.c_str()
                );

                return false;
            }
        }

        for( const PackageFontDesc & desc : m_fontsDesc )
        {
            if( desc.platform.empty() == false && desc.platform.hasTags( platformTags ) == false )
            {
                continue;
            }

            if( this->loadFont_( desc.path ) == false )
            {
                LOGGER_ERROR( "file '%s' package '%s' invalid load font '%s'"
                    , m_desc.path.c_str()
                    , m_desc.name.c_str()
                    , desc.path.c_str()
                );

                return false;
            }
        }

        for( const PackageTextDesc & desc : m_textsDesc )
        {
            if( desc.platform.empty() == false && desc.platform.hasTags( platformTags ) == false )
            {
                continue;
            }

            if( this->loadText_( desc.path ) == false )
            {
                LOGGER_ERROR( "file '%s' package '%s' invalid load text '%s'"
                    , m_desc.path.c_str()
                    , m_desc.name.c_str()
                    , desc.path.c_str()
                );

                return false;
            }
        }

        for( const PackageDataDesc & desc : m_datasDesc )
        {
            if( desc.platform.empty() == false && desc.platform.hasTags( platformTags ) == false )
            {
                continue;
            }

            if( this->loadData_( desc.name, desc.path ) == false )
            {
                LOGGER_ERROR( "file '%s' package '%s' invalid load userdata '%s' path '%s'"
                    , m_desc.path.c_str()
                    , m_desc.name.c_str()
                    , desc.name.c_str()
                    , desc.path.c_str()
                );

                return false;
            }
        }

        for( const PackageMaterialDesc & desc : m_materialsDesc )
        {
            if( desc.platform.empty() == false && desc.platform.hasTags( platformTags ) == false )
            {
                continue;
            }

            if( this->loadMaterials_( m_fileGroup, desc.path ) == false )
            {
                LOGGER_ERROR( "file '%s' package '%s' invalid load material '%s'"
                    , m_desc.path.c_str()
                    , m_desc.name.c_str()
                    , desc.path.c_str()
                );

                return false;
            }
        }

        for( const PackageSettingDesc & desc : m_settingsDesc )
        {
            if( desc.platform.empty() == false && desc.platform.hasTags( platformTags ) == false )
            {
                continue;
            }

            if( this->loadSetting_( desc.name, desc.path ) == false )
            {
                LOGGER_ERROR( "file '%s' package '%s' invalid load setting '%s' path '%s'"
                    , m_desc.path.c_str()
                    , m_desc.name.c_str()
                    , desc.name.c_str()
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
            LOGGER_ERROR( "file '%s' package '%s' already disable '%s'"
                , m_desc.path.c_str()
                , m_desc.name.c_str()
                , m_desc.descriptionPath.c_str()
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

        for( const PackageResourceDesc & desc : m_resourcesDesc )
        {
            if( desc.platform.empty() == false && desc.platform.hasTags( platformTags ) == false )
            {
                continue;
            }

            if( this->unloadResources_( m_desc.locale, m_fileGroup, desc.path ) == false )
            {
                return false;
            }
        }

        for( const PackageFontDesc & desc : m_fontsDesc )
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

        for( const PackageTextDesc & desc : m_textsDesc )
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

        for( const PackageDataDesc & desc : m_datasDesc )
        {
            if( desc.platform.empty() == false && desc.platform.hasTags( platformTags ) == false )
            {
                continue;
            }

            if( this->unloadData_( desc.name ) == false )
            {
                return false;
            }
        }

        for( const PackageMaterialDesc & desc : m_materialsDesc )
        {
            if( desc.platform.empty() == false && desc.platform.hasTags( platformTags ) == false )
            {
                continue;
            }

            if( this->unloadMaterials_( m_fileGroup, desc.path ) == false )
            {
                return false;
            }
        }

        for( const PackageSettingDesc & desc : m_settingsDesc )
        {
            if( desc.platform.empty() == false && desc.platform.hasTags( platformTags ) == false )
            {
                continue;
            }

            if( this->unloadSetting_( desc.name ) == false )
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
    bool Package::loadText_( const FilePath & _filePath )
    {
        bool successful = TEXT_SERVICE()
            ->loadTextEntry( m_desc.locale, m_fileGroup, _filePath );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Package::unloadText_( const FilePath & _filePath )
    {
        bool successful = TEXT_SERVICE()
            ->unloadTextEntry( m_desc.locale, m_fileGroup, _filePath );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Package::loadFont_( const FilePath & _filePath )
    {
        bool successful = FONT_SERVICE()
            ->loadFonts( m_fileGroup, _filePath );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Package::unloadFont_( const FilePath & _filePath )
    {
        bool successful = FONT_SERVICE()
            ->unloadFonts( m_fileGroup, _filePath );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Package::loadData_( const ConstString & _name, const FilePath & _filePath )
    {
        bool successful = USERDATA_SERVICE()
            ->addUserdata( _name, m_fileGroup, _filePath, MENGINE_DOCUMENT_FACTORABLE );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Package::unloadData_( const ConstString & _name )
    {
        bool successful = USERDATA_SERVICE()
            ->removeUserdata( _name );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Package::loadSetting_( const ConstString & _name, const FilePath & _filePath )
    {
        bool successful = SETTINGS_SERVICE()
            ->loadSetting( _name, m_fileGroup, _filePath, MENGINE_DOCUMENT_FACTORABLE );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Package::unloadSetting_( const ConstString & _name )
    {
        bool successful = SETTINGS_SERVICE()
            ->unloadSetting( _name );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void Package::addResource_( const FilePath & _filePath, const Tags & _tags, const Tags & _platform, bool _demand, bool _ignored )
    {
        PackageResourceDesc desc;
        desc.path = _filePath;
        desc.tags = _tags;
        desc.platform = _platform;
        desc.demand = _demand;
        desc.ignored = _ignored;

        m_resourcesDesc.emplace_back( desc );
    }
    //////////////////////////////////////////////////////////////////////////
    void Package::addTextPath_( const FilePath & _filePath, const Tags & _platform )
    {
        PackageTextDesc desc;
        desc.path = _filePath;
        desc.platform = _platform;

        m_textsDesc.emplace_back( desc );
    }
    //////////////////////////////////////////////////////////////////////////
    void Package::addScriptPackage_( const FilePath & _filePath, const ConstString & _module, const ConstString & _initializer, const ConstString & _finalizer, const Tags & _platform )
    {
        ScriptModulePackage package;
        package.fileGroup = m_fileGroup;
        package.path = _filePath;
        package.module = _module;
        package.initializer = _initializer;
        package.finalizer = _finalizer;
        package.platform = _platform;

        m_scriptsPackages.emplace_back( package );
    }
    //////////////////////////////////////////////////////////////////////////
    void Package::addFontPath_( const FilePath & _filePath, const Tags & _platform )
    {
        PackageFontDesc desc;
        desc.path = _filePath;
        desc.platform = _platform;

        m_fontsDesc.emplace_back( desc );
    }
    //////////////////////////////////////////////////////////////////////////
    void Package::addData_( const ConstString & _name, const FilePath & _filePath, const Tags & _platform )
    {
        PackageDataDesc desc;
        desc.name = _name;
        desc.path = _filePath;
        desc.platform = _platform;

        m_datasDesc.emplace_back( desc );
    }
    //////////////////////////////////////////////////////////////////////////
    void Package::addMaterial_( const FilePath & _filePath, const Tags & _platform )
    {
        PackageMaterialDesc desc;
        desc.path = _filePath;
        desc.platform = _platform;

        m_materialsDesc.emplace_back( desc );
    }
    //////////////////////////////////////////////////////////////////////////
    void Package::addSetting_( const ConstString & _name, const FilePath & _filePath, const Tags & _platform )
    {
        PackageSettingDesc desc;
        desc.name = _name;
        desc.path = _filePath;
        desc.platform = _platform;

        m_settingsDesc.emplace_back( desc );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Package::loadResources_( const ConstString & _locale, const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const Tags & _tags, bool _ignored )
    {
        Metacode::Meta_Data::Meta_DataBlock datablock;

        bool exist = false;
        if( LOADER_SERVICE()
            ->load( _fileGroup, _filePath, &datablock, Metacode::Meta_Data::getVersion(), &exist ) == false )
        {
            if( exist == false )
            {
                LOGGER_ERROR( "resource '%s' not found"
                    , Helper::getFileGroupFullPath( _fileGroup, _filePath )
                );
            }
            else
            {
                LOGGER_ERROR( "invalid parse resource '%s'"
                    , Helper::getFileGroupFullPath( _fileGroup, _filePath )
                );
            }

            return false;
        }

        const ConstString & groupName = datablock.get_Name();

        const Metacode::Meta_Data::Meta_DataBlock::VectorMeta_Resource & includes_resource = datablock.get_Includes_Resource();

        for( const Metacode::Meta_Data::Meta_DataBlock::Meta_Resource * meta_resource : includes_resource )
        {
            const ConstString & name = meta_resource->get_Name();
            const ConstString & type = meta_resource->get_Type();

            //ToDo
            bool unique = meta_resource->getd_Unique( true );

            ResourcePtr has_resource;
            if( RESOURCE_SERVICE()
                ->hasResource( groupName, name, true, &has_resource ) == true )
            {
                if( unique == false )
                {
                    continue;
                }

                LOGGER_ERROR( "file '%s' already exist resource name '%s' in group '%s'\nhas resource group '%s' name '%s'"
                    , Helper::getFileGroupFullPath( _fileGroup, _filePath )
                    , name.c_str()
                    , groupName.c_str()
                    , has_resource->getGroupName().c_str()
                    , has_resource->getName().c_str()
                );

                return false;
            }

            ResourcePtr resource = RESOURCE_SERVICE()
                ->createResource( _locale, groupName, name, type, true, true, MENGINE_DOCUMENT_MESSAGE( "locale '%s' group '%s' name '%s' type '%s'", _locale.c_str(), groupName.c_str(), name.c_str(), type.c_str() ) );

            MENGINE_ASSERTION_MEMORY_PANIC( resource, "file '%s' group '%s' resource '%s' type '%s' invalid create"
                , Helper::getFileGroupFullPath( _fileGroup, _filePath )
                , groupName.c_str()
                , name.c_str()
                , type.c_str()
            );

            resource->setTags( _tags );

            ContentInterfacePtr content = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "FileContent" ), ConstString::none(), MENGINE_DOCUMENT_FACTORABLE );

            content->setFileGroup( _fileGroup );

            resource->setContent( content );
            
            LoaderInterfacePtr loader = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Loader" ), type );

            MENGINE_ASSERTION_MEMORY_PANIC( loader, "file '%s' group '%s' resource '%s' type '%s' invalid create loader"
                , Helper::getFileGroupFullPath( _fileGroup, _filePath )
                , groupName.c_str()
                , resource->getName().c_str()
                , resource->getType().c_str()
            );

            if( loader->load( resource, meta_resource ) == false )
            {
                LOGGER_ERROR( "file '%s' group '%s' resource '%s' type '%s' invalid load"
                    , Helper::getFileGroupFullPath( _fileGroup, _filePath )
                    , groupName.c_str()
                    , resource->getName().c_str()
                    , resource->getType().c_str()
                );

                return false;
            }

            resource->setIgnored( _ignored );

            if( resource->initialize() == false )
            {
                LOGGER_ERROR( "file '%s' group '%s' resource '%s' type '%s' invalid initialize"
                    , Helper::getFileGroupFullPath( _fileGroup, _filePath )
                    , groupName.c_str()
                    , resource->getName().c_str()
                    , resource->getType().c_str()
                );

                return false;
            }

            bool precompile = false;
            meta_resource->get_Precompile( &precompile );

            if( precompile == true )
            {
                resource->setPrecompile( true );

                if( resource->compile() == false )
                {
                    LOGGER_ERROR( "resource '%s' type [%s] invalid precompile"
                        , name.c_str()
                        , type.c_str()
                    );

                    continue;
                }
            }
        }

        const Metacode::Meta_Data::Meta_DataBlock::VectorMeta_Include & includes_include = datablock.get_Includes_Include();

        for( const Metacode::Meta_Data::Meta_DataBlock::Meta_Include & meta_include : includes_include )
        {
            const FilePath & path = meta_include.get_Path();

            if( this->loadResources_( _locale, _fileGroup, path, _tags, _ignored ) == false )
            {
                LOGGER_ERROR( "load '%s' resource invalid load include '%s'"
                    , Helper::getFileGroupFullPath( _fileGroup, _filePath )
                    , path.c_str()
                );

                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Package::unloadResources_( const ConstString & _locale, const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath )
    {
        Metacode::Meta_Data::Meta_DataBlock datablock;

        bool exist = false;
        if( LOADER_SERVICE()
            ->load( _fileGroup, _filePath, &datablock, Metacode::Meta_Data::getVersion(), &exist ) == false )
        {
            if( exist == false )
            {
                LOGGER_ERROR( "file resources '%s' not found"
                    , Helper::getFileGroupFullPath( _fileGroup, _filePath )
                );
            }
            else
            {
                LOGGER_ERROR( "invalid parse resources '%s'"
                    , Helper::getFileGroupFullPath( _fileGroup, _filePath )
                );
            }

            return false;
        }

        const ConstString & groupName = datablock.get_Name();

        const Metacode::Meta_Data::Meta_DataBlock::VectorMeta_Include & includes_include = datablock.get_Includes_Include();

        for( const Metacode::Meta_Data::Meta_DataBlock::Meta_Include & meta_include : includes_include )
        {
            const FilePath & path = meta_include.get_Path();

            if( this->unloadResources_( _locale, _fileGroup, path ) == false )
            {
                LOGGER_ERROR( "load '%s' resource invalid load include '%s'"
                    , Helper::getFileGroupFullPath( _fileGroup, _filePath )
                    , path.c_str()
                );

                return false;
            }
        }

        const Metacode::Meta_Data::Meta_DataBlock::VectorMeta_Resource & includes_resource = datablock.get_Includes_Resource();

        for( const Metacode::Meta_Data::Meta_DataBlock::Meta_Resource * meta_resource : includes_resource )
        {
            const ConstString & name = meta_resource->get_Name();

            ResourcePtr has_resource = nullptr;
            if( RESOURCE_SERVICE()
                ->hasResource( groupName, name, false, &has_resource ) == false )
            {
                LOGGER_ERROR( "path '%s' not found resource name '%s' in group '%s'\nhas resource group '%s' name '%s'"
                    , Helper::getFileGroupFullPath( _fileGroup, _filePath )
                    , meta_resource->get_Name().c_str()
                    , groupName.c_str()
                    , has_resource->getGroupName().c_str()
                    , has_resource->getName().c_str()
                );

                return false;
            }

            bool precompile = false;
            meta_resource->get_Precompile( &precompile );

            if( precompile == true )
            {
                has_resource->release();
            }

            RESOURCE_SERVICE()
                ->removeResource( has_resource );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Package::loadMaterials_( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath )
    {
        Metacode::Meta_Data::Meta_DataBlock datablock;

        bool exist = false;
        if( LOADER_SERVICE()
            ->load( _fileGroup, _filePath, &datablock, Metacode::Meta_Data::getVersion(), &exist ) == false )
        {
            if( exist == false )
            {
                LOGGER_ERROR( "materials '%s' not found"
                    , Helper::getFileGroupFullPath( _fileGroup, _filePath )
                );
            }
            else
            {
                LOGGER_ERROR( "invalid parse materials '%s'"
                    , Helper::getFileGroupFullPath( _fileGroup, _filePath )
                );
            }

            return false;
        }

        const ConstString & renderPlatformName = RENDER_SYSTEM()
            ->getRenderPlatformName();

        const Metacode::Meta_Data::Meta_DataBlock::VectorMeta_FragmentShader & includes_FragmentShader = datablock.get_Includes_FragmentShader();

        for( const Metacode::Meta_Data::Meta_DataBlock::Meta_FragmentShader & meta_FragmentShader : includes_FragmentShader )
        {
            const ConstString & name = meta_FragmentShader.get_Name();
            const ConstString & renderPlatform = meta_FragmentShader.get_RenderPlatform();

            if( renderPlatform != renderPlatformName )
            {
                continue;
            }

            const FilePath & fragmentShaderFilePath = meta_FragmentShader.get_File_Path();

            ConstString fileConverterType;
            meta_FragmentShader.get_File_Converter( &fileConverterType );

            bool isCompile = false;
            meta_FragmentShader.get_File_Compile( &isCompile );

            RenderFragmentShaderInterfacePtr shader = RENDERMATERIAL_SERVICE()
                ->createFragmentShader( name, _fileGroup, fragmentShaderFilePath, fileConverterType, isCompile, MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( shader, "material '%s' invalid load '%s' fragment shader '%s' compile [%u]"
                , Helper::getFileGroupFullPath( _fileGroup, _filePath )
                , Helper::getFileGroupFullPath( _fileGroup, fragmentShaderFilePath )
                , name.c_str()
                , isCompile
            );

            MENGINE_UNUSED( shader );
        }

        const Metacode::Meta_Data::Meta_DataBlock::VectorMeta_VertexShader & includes_VertexShader = datablock.get_Includes_VertexShader();

        for( const Metacode::Meta_Data::Meta_DataBlock::Meta_VertexShader & meta_VertexShader : includes_VertexShader )
        {
            const ConstString & renderPlatform = meta_VertexShader.get_RenderPlatform();

            if( renderPlatform != renderPlatformName )
            {
                continue;
            }

            const ConstString & name = meta_VertexShader.get_Name();

            const FilePath & filePath = meta_VertexShader.get_File_Path();

            ConstString fileConverter;
            meta_VertexShader.get_File_Converter( &fileConverter );

            bool isCompile = false;
            meta_VertexShader.get_File_Compile( &isCompile );

            RenderVertexShaderInterfacePtr shader = RENDERMATERIAL_SERVICE()
                ->createVertexShader( name, _fileGroup, filePath, fileConverter, isCompile, MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( shader, "material '%s' invalid load '%s' vertex shader '%s' compile [%u]"
                , Helper::getFileGroupFullPath( _fileGroup, _filePath )
                , filePath.c_str()
                , name.c_str()
                , isCompile
            );

            MENGINE_UNUSED( shader );
        }

        const Metacode::Meta_Data::Meta_DataBlock::VectorMeta_VertexAttribute & includes_VertexAttribute = datablock.get_Includes_VertexAttribute();

        for( const Metacode::Meta_Data::Meta_DataBlock::Meta_VertexAttribute & meta_VertexAttribute : includes_VertexAttribute )
        {
            const ConstString & renderPlatform = meta_VertexAttribute.get_RenderPlatform();

            if( renderPlatform != renderPlatformName )
            {
                continue;
            }

            uint32_t elementSize = meta_VertexAttribute.get_Element_Size();

            const ConstString & name = meta_VertexAttribute.get_Name();

            RenderVertexAttributeInterfacePtr vertexAttribute = RENDERMATERIAL_SERVICE()
                ->createVertexAttribute( name, elementSize, MENGINE_DOCUMENT_FACTORABLE );

            const Metacode::Meta_Data::Meta_DataBlock::Meta_VertexAttribute::VectorMeta_Attribute & includes_Attributes = meta_VertexAttribute.get_Includes_Attribute();

            for( const Metacode::Meta_Data::Meta_DataBlock::Meta_VertexAttribute::Meta_Attribute & meta_Attribute : includes_Attributes )
            {
                const ConstString & attribute_uniform = meta_Attribute.get_Uniform();
                uint32_t attribute_index = meta_Attribute.get_Index();
                uint32_t attribute_size = meta_Attribute.get_Size();
                EVertexAttributeType attribute_type = meta_Attribute.get_Type();
                bool attribute_normalized = meta_Attribute.get_Normalized();
                uint32_t attribute_stride = meta_Attribute.get_Stride();
                uint32_t attribute_offset = meta_Attribute.get_Offset();

                vertexAttribute->addAttribute( attribute_uniform, attribute_index, attribute_size, attribute_type, attribute_normalized, attribute_stride, attribute_offset );
            }
        }

        const Metacode::Meta_Data::Meta_DataBlock::VectorMeta_Program & includes_Program = datablock.get_Includes_Program();

        for( const Metacode::Meta_Data::Meta_DataBlock::Meta_Program & meta_Program : includes_Program )
        {
            ConstString renderPlatform;
            if( meta_Program.get_RenderPlatform( &renderPlatform ) == true )
            {
                if( renderPlatform != renderPlatformName )
                {
                    continue;
                }
            }

            const ConstString & name = meta_Program.get_Name();

            const ConstString & vertexShaderName = meta_Program.get_VertexShader_Name();
            const ConstString & fragmentShaderName = meta_Program.get_FragmentShader_Name();
            const ConstString & vertexAttributeName = meta_Program.get_VertexAttribute_Name();
            uint32_t samplerCount = meta_Program.get_Sampler_Count();

            const RenderVertexShaderInterfacePtr & vertexShader = RENDERMATERIAL_SERVICE()
                ->getVertexShader( vertexShaderName );

            MENGINE_ASSERTION_MEMORY_PANIC( vertexShader, "material '%s' program '%s' not found vertex shader '%s'"
                , Helper::getFileGroupFullPath( _fileGroup, _filePath )
                , name.c_str()
                , vertexShaderName.c_str()
            );

            const RenderFragmentShaderInterfacePtr & fragmentShader = RENDERMATERIAL_SERVICE()
                ->getFragmentShader( fragmentShaderName );

            MENGINE_ASSERTION_MEMORY_PANIC( fragmentShader, "material '%s' program '%s' not found fragment shader '%s'"
                , Helper::getFileGroupFullPath( _fileGroup, _filePath )
                , name.c_str()
                , fragmentShaderName.c_str()
            );

            const RenderVertexAttributeInterfacePtr & vertexAttribute = RENDERMATERIAL_SERVICE()
                ->getVertexAttribute( vertexAttributeName );

            MENGINE_ASSERTION_MEMORY_PANIC( vertexAttribute, "material '%s' program '%s' not found vertex attribute '%s'"
                , Helper::getFileGroupFullPath( _fileGroup, _filePath )
                , name.c_str()
                , vertexAttributeName.c_str()
            );

            RenderProgramInterfacePtr program = RENDERMATERIAL_SERVICE()
                ->createProgram( name, vertexShader, fragmentShader, vertexAttribute, samplerCount, MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( program, "material '%s' invalid create program vertex '%s' fragment '%s'"
                , Helper::getFileGroupFullPath( _fileGroup, _filePath )
                , vertexShaderName.c_str()
                , fragmentShaderName.c_str()
            );

            MENGINE_UNUSED( program );
        }

        const Metacode::Meta_Data::Meta_DataBlock::VectorMeta_Material & includes_Material = datablock.get_Includes_Material();

        for( const Metacode::Meta_Data::Meta_DataBlock::Meta_Material & meta_Material : includes_Material )
        {
            const ConstString & name = meta_Material.get_Name();

            ConstString renderPlatform;
            if( meta_Material.get_RenderPlatform( &renderPlatform ) == true )
            {
                if( renderPlatform != renderPlatformName )
                {
                    continue;
                }
            }

            bool is_debug = false;
            meta_Material.get_Debug( &is_debug );

            RenderMaterialStage stage;
            stage.separateAlphaBlendEnable = meta_Material.get_SeparateAlphaBlend_Enable();
            stage.alphaBlendEnable = meta_Material.get_AlphaBlend_Enable();
            stage.depthBufferTestEnable = meta_Material.get_DepthBufferTest_Enable();
            stage.depthBufferWriteEnable = meta_Material.get_DepthBufferWrite_Enable();

            meta_Material.get_BlendFactor_Source( &stage.blendSrc );
            meta_Material.get_BlendFactor_Dest( &stage.blendDst );
            meta_Material.get_BlendFactor_Op( &stage.blendOp );

            meta_Material.get_SeparateAlphaBlendFactor_Source( &stage.separateAlphaBlendSrc );
            meta_Material.get_SeparateAlphaBlendFactor_Dest( &stage.separateAlphaBlendDst );
            meta_Material.get_SeparateAlphaBlendFactor_Op( &stage.separateAlphaBlendOp );

            ConstString programName;
            if( meta_Material.get_Program_Name( &programName ) == true )
            {
                const RenderProgramInterfacePtr & program = RENDERMATERIAL_SERVICE()
                    ->getProgram( programName );

                MENGINE_ASSERTION_MEMORY_PANIC( program, "material '%s' invalid get program '%s'"
                    , Helper::getFileGroupFullPath( _fileGroup, _filePath )
                    , programName.c_str()
                );

                stage.program = program;
            }

            const Metacode::Meta_Data::Meta_DataBlock::Meta_Material::VectorMeta_TextureStages & include_TextureStages = meta_Material.get_Includes_TextureStages();

            for( uint32_t index = 0; index != MENGINE_MAX_TEXTURE_STAGES; ++index )
            {
                RenderTextureStage & textureStage = stage.textureStages[index];

                textureStage.mipmap = RENDERMATERIAL_SERVICE()
                    ->getDefaultTextureFilterMipmap();

                textureStage.magnification = RENDERMATERIAL_SERVICE()
                    ->getDefaultTextureFilterMagnification();

                textureStage.minification = RENDERMATERIAL_SERVICE()
                    ->getDefaultTextureFilterMinification();
            }

            for( const Metacode::Meta_Data::Meta_DataBlock::Meta_Material::Meta_TextureStages & meta_TextureStages : include_TextureStages )
            {
                uint32_t index = meta_TextureStages.get_Stage();

                RenderTextureStage & textureStage = stage.textureStages[index];

                meta_TextureStages.get_AddressMode_U( &textureStage.addressU );
                meta_TextureStages.get_AddressMode_V( &textureStage.addressV );
                meta_TextureStages.get_AddressMode_Border( &textureStage.addressBorder );
            }

            const RenderMaterialStage * cache_stage = RENDERMATERIAL_SERVICE()
                ->createMaterialStage( name, stage, MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( cache_stage, "material '%s' invalid create stage group '%s'"
                , Helper::getFileGroupFullPath( _fileGroup, _filePath )
                , name.c_str()
            );

            MENGINE_UNUSED( cache_stage );

            if( is_debug == true )
            {
                const RenderMaterialInterfacePtr & debugLineMaterial = RENDERMATERIAL_SERVICE()
                    ->getMaterial( name, PT_LINELIST, nullptr, 0, MENGINE_DOCUMENT_FACTORABLE );

                RENDERMATERIAL_SERVICE()
                    ->setDebugLineMaterial( debugLineMaterial );

                const RenderMaterialInterfacePtr & debugTriangleMaterial = RENDERMATERIAL_SERVICE()
                    ->getMaterial( name, PT_TRIANGLELIST, nullptr, 0, MENGINE_DOCUMENT_FACTORABLE );

                RENDERMATERIAL_SERVICE()
                    ->setDebugTriangleMaterial( debugTriangleMaterial );
            }
        }

        RENDERMATERIAL_SERVICE()
            ->updateSolidRenderMaterial();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Package::unloadMaterials_( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath )
    {
        Metacode::Meta_Data::Meta_DataBlock datablock;

        bool exist = false;
        if( LOADER_SERVICE()
            ->load( _fileGroup, _filePath, &datablock, Metacode::Meta_Data::getVersion(), &exist ) == false )
        {
            if( exist == false )
            {
                LOGGER_ERROR( "materials '%s' not found"
                    , Helper::getFileGroupFullPath( _fileGroup, _filePath )
                );
            }
            else
            {
                LOGGER_ERROR( "invalid parse materials '%s'"
                    , Helper::getFileGroupFullPath( _fileGroup, _filePath )
                );
            }

            return false;
        }

        const ConstString & renderPlatformName = RENDER_SYSTEM()
            ->getRenderPlatformName();

        const Metacode::Meta_Data::Meta_DataBlock::VectorMeta_FragmentShader & includes_FragmentShader = datablock.get_Includes_FragmentShader();

        for( const Metacode::Meta_Data::Meta_DataBlock::Meta_FragmentShader & meta_FragmentShader : includes_FragmentShader )
        {
            const ConstString & renderPlatform = meta_FragmentShader.get_RenderPlatform();

            if( renderPlatform != renderPlatformName )
            {
                continue;
            }

            const ConstString & name = meta_FragmentShader.get_Name();

            RENDERMATERIAL_SERVICE()
                ->removeFragmentShader( name );            
        }

        const Metacode::Meta_Data::Meta_DataBlock::VectorMeta_VertexShader & includes_VertexShader = datablock.get_Includes_VertexShader();

        for( const Metacode::Meta_Data::Meta_DataBlock::Meta_VertexShader & meta_VertexShader : includes_VertexShader )
        {
            const ConstString & renderPlatform = meta_VertexShader.get_RenderPlatform();

            if( renderPlatform != renderPlatformName )
            {
                continue;
            }

            const ConstString & name = meta_VertexShader.get_Name();

            RENDERMATERIAL_SERVICE()
                ->removeVertexShader( name );
        }

        const Metacode::Meta_Data::Meta_DataBlock::VectorMeta_Program & includes_Program = datablock.get_Includes_Program();

        for( const Metacode::Meta_Data::Meta_DataBlock::Meta_Program & meta_Program : includes_Program )
        {
            ConstString renderPlatform;
            if( meta_Program.get_RenderPlatform( &renderPlatform ) == true )
            {
                if( renderPlatform != renderPlatformName )
                {
                    continue;
                }
            }

            const ConstString & name = meta_Program.get_Name();

            RENDERMATERIAL_SERVICE()
                ->removeProgram( name );
        }

        const Metacode::Meta_Data::Meta_DataBlock::VectorMeta_Material & includes_Material = datablock.get_Includes_Material();

        for( const Metacode::Meta_Data::Meta_DataBlock::Meta_Material & meta_Material : includes_Material )
        {
            const ConstString & name = meta_Material.get_Name();

            RENDERMATERIAL_SERVICE()
                ->removeMaterialStage( name );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}