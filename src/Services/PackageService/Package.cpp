#include "Package.h"

#include "Interface/LoaderServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/TextServiceInterface.h"
#include "Interface/FontServiceInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/UnicodeSystemInterface.h"
#include "Interface/UserdataServiceInterface.h"
#include "Interface/RenderSystemInterface.h"
#include "Interface/PlatformServiceInterface.h"
#include "Interface/RenderMaterialServiceInterface.h"
#include "Interface/SettingsServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"

#include "Metacode/Metacode.h"

#include "Kernel/Logger.h"
#include "Kernel/Resource.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/FileGroupHelper.h"
#include "Kernel/ContentHelper.h"
#include "Kernel/VocabularyHelper.h"

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
        m_glyphsDesc.clear();
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
    bool Package::load( const DocumentInterfacePtr & _doc )
    {
        if( this->mountFileGroup_( _doc ) == false )
        {
            return false;
        }

        if( this->loadPackage_( _doc ) == false )
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
    bool Package::mountFileGroup_( const DocumentInterfacePtr & _doc )
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
        if( m_fileGroup != nullptr )
        {
            FILE_SERVICE()
                ->unmountFileGroup(m_desc.name);

            m_fileGroup = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Package::loadPackage_( const DocumentInterfacePtr & _doc )
    {
        if( m_desc.descriptionPath.empty() == true )
        {
            return true;
        }

        m_resourcesDesc.reserve( 32 );

        PackageLoaderInterfacePtr loader = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "PackageLoader" ), m_desc.format );

        MENGINE_ASSERTION_MEMORY_PANIC( loader, "invalid get package loader '%s'"
            , m_desc.format.c_str()
        );
         
        if( loader->load( PackageInterfacePtr::from( this ), _doc ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Package::enable()
    {
        LOGGER_INFO( "package", "package enable... '%s'"
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

        if( SERVICE_IS_INITIALIZE( ScriptServiceInterface ) == true )
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

            if( this->enableResources_( m_desc.locales, desc ) == false )
            {
                LOGGER_ERROR( "invalid load file '%s' name '%s' resource '%s'"
                    , m_desc.path.c_str()
                    , m_desc.name.c_str()
                    , Helper::getContentFullPath( desc.content ).c_str()
                );

                return false;
            }
        }

        for( const PackageGlyphDesc & desc : m_glyphsDesc )
        {
            if( desc.platform.empty() == false && desc.platform.hasTags( platformTags ) == false )
            {
                continue;
            }

            if( this->enableGlyph_( desc ) == false )
            {
                LOGGER_ERROR( "file '%s' package '%s' invalid load glyph '%s'"
                    , m_desc.path.c_str()
                    , m_desc.name.c_str()
                    , Helper::getContentFullPath( desc.content ).c_str()
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

            if( this->enableFont_( desc ) == false )
            {
                LOGGER_ERROR( "file '%s' package '%s' invalid load font '%s'"
                    , m_desc.path.c_str()
                    , m_desc.name.c_str()
                    , Helper::getContentFullPath( desc.content ).c_str()
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

            if( this->enableText_( desc ) == false )
            {
                LOGGER_ERROR( "file '%s' package '%s' invalid load text '%s'"
                    , m_desc.path.c_str()
                    , m_desc.name.c_str()
                    , Helper::getContentFullPath( desc.content ).c_str()
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

            if( this->enableData_( desc ) == false )
            {
                LOGGER_ERROR( "file '%s' package '%s' invalid load userdata '%s' path '%s'"
                    , m_desc.path.c_str()
                    , m_desc.name.c_str()
                    , desc.name.c_str()
                    , Helper::getContentFullPath( desc.content ).c_str()
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

            if( this->enableMaterials_( desc ) == false )
            {
                LOGGER_ERROR( "file '%s' package '%s' invalid load material '%s'"
                    , m_desc.path.c_str()
                    , m_desc.name.c_str()
                    , Helper::getContentFullPath( desc.content ).c_str()
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

            if( this->enableSetting_( desc ) == false )
            {
                LOGGER_ERROR( "file '%s' package '%s' invalid load setting '%s' path '%s'"
                    , m_desc.path.c_str()
                    , m_desc.name.c_str()
                    , desc.name.c_str()
                    , Helper::getContentFullPath( desc.content ).c_str()
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

        LOGGER_INFO( "package", "package disable... '%s'"
            , m_desc.name.c_str()
        );

        m_enable = false;

        if( SERVICE_IS_INITIALIZE( ScriptServiceInterface ) == true )
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

            if( this->disableResources_( m_desc.locales, desc ) == false )
            {
                return false;
            }
        }

        for( const PackageGlyphDesc & desc : m_glyphsDesc )
        {
            if( desc.platform.empty() == false && desc.platform.hasTags( platformTags ) == false )
            {
                continue;
            }

            if( this->disableGlyph_( desc ) == false )
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

            if( this->disableFont_( desc ) == false )
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

            if( this->disableText_( desc ) == false )
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

            if( this->disableData_( desc ) == false )
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

            if( this->disableMaterials_( desc ) == false )
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

            if( this->disableSetting_( desc ) == false )
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
    bool Package::enableText_( const PackageTextDesc & _desc )
    {
        bool successful = TEXT_SERVICE()
            ->loadTextEntry( m_desc.locales, _desc.content, MENGINE_DOCUMENT_FACTORABLE );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Package::disableText_( const PackageTextDesc & _desc )
    {
        bool successful = TEXT_SERVICE()
            ->unloadTextEntry( m_desc.locales, _desc.content );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Package::enableGlyph_( const PackageGlyphDesc & _desc )
    {
        bool successful = FONT_SERVICE()
            ->loadGlyphs( _desc.content, MENGINE_DOCUMENT_FACTORABLE );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Package::disableGlyph_( const PackageGlyphDesc & _desc )
    {
        bool successful = FONT_SERVICE()
            ->unloadGlyphs( _desc.content );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Package::enableFont_( const PackageFontDesc & _desc )
    {
        bool successful = FONT_SERVICE()
            ->loadFonts( _desc.content, MENGINE_DOCUMENT_FACTORABLE );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Package::disableFont_( const PackageFontDesc & _desc )
    {
        bool successful = FONT_SERVICE()
            ->unloadFonts( _desc.content );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Package::enableData_( const PackageDataDesc & _desc )
    {
        bool successful = USERDATA_SERVICE()
            ->addUserdata( _desc.name, _desc.content, MENGINE_DOCUMENT_FACTORABLE );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Package::disableData_( const PackageDataDesc & _desc )
    {
        bool successful = USERDATA_SERVICE()
            ->removeUserdata( _desc.name );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Package::enableSetting_( const PackageSettingDesc & _desc )
    {
        bool successful = SETTINGS_SERVICE()
            ->loadSetting( _desc.name, _desc.content, MENGINE_DOCUMENT_FACTORABLE );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Package::disableSetting_( const PackageSettingDesc & _desc )
    {
        bool successful = SETTINGS_SERVICE()
            ->unloadSetting( _desc.name );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void Package::addPackageResource( const FilePath & _filePath, const Tags & _tags, const Tags & _platform, bool _demand, bool _ignored )
    {
        ContentInterfacePtr content = Helper::makeFileContent( m_fileGroup, _filePath, MENGINE_DOCUMENT_FACTORABLE );

        PackageResourceDesc desc;
        desc.content = content;
        desc.tags = _tags;
        desc.platform = _platform;
        desc.demand = _demand;
        desc.ignored = _ignored;

        m_resourcesDesc.emplace_back( desc );
    }
    //////////////////////////////////////////////////////////////////////////
    void Package::addPackageTextPath( const FilePath & _filePath, const Tags & _platform )
    {
        ContentInterfacePtr content = Helper::makeFileContent( m_fileGroup, _filePath, MENGINE_DOCUMENT_FACTORABLE );

        PackageTextDesc desc;
        desc.content = content;
        desc.platform = _platform;

        m_textsDesc.emplace_back( desc );
    }
    //////////////////////////////////////////////////////////////////////////
    void Package::addPackageScript( const FilePath & _filePath, const ConstString & _module, const ConstString & _initializer, const ConstString & _finalizer, const Tags & _platform )
    {
        ContentInterfacePtr content = Helper::makeFileContent( m_fileGroup, _filePath, MENGINE_DOCUMENT_FACTORABLE );

        ScriptModulePackage package;
        package.content = content;
        package.module = _module;
        package.initializer = _initializer;
        package.finalizer = _finalizer;
        package.platform = _platform;

        m_scriptsPackages.emplace_back( package );
    }
    //////////////////////////////////////////////////////////////////////////
    void Package::addPackageGlyphPath( const FilePath & _filePath, const Tags & _platform )
    {
        ContentInterfacePtr content = Helper::makeFileContent( m_fileGroup, _filePath, MENGINE_DOCUMENT_FACTORABLE );

        PackageGlyphDesc desc;
        desc.content = content;
        desc.platform = _platform;

        m_glyphsDesc.emplace_back( desc );
    }
    //////////////////////////////////////////////////////////////////////////
    void Package::addPackageFontPath( const FilePath & _filePath, const Tags & _platform )
    {
        ContentInterfacePtr content = Helper::makeFileContent( m_fileGroup, _filePath, MENGINE_DOCUMENT_FACTORABLE );

        PackageFontDesc desc;
        desc.content = content;
        desc.platform = _platform;

        m_fontsDesc.emplace_back( desc );
    }
    //////////////////////////////////////////////////////////////////////////
    void Package::addPackageData( const ConstString & _name, const FilePath & _filePath, const Tags & _platform )
    {
        ContentInterfacePtr content = Helper::makeFileContent( m_fileGroup, _filePath, MENGINE_DOCUMENT_FACTORABLE );

        PackageDataDesc desc;
        desc.name = _name;
        desc.content = content;
        desc.platform = _platform;

        m_datasDesc.emplace_back( desc );
    }
    //////////////////////////////////////////////////////////////////////////
    void Package::addPackageMaterial( const FilePath & _filePath, const Tags & _platform )
    {
        ContentInterfacePtr content = Helper::makeFileContent( m_fileGroup, _filePath, MENGINE_DOCUMENT_FACTORABLE );

        PackageMaterialDesc desc;
        desc.content = content;
        desc.platform = _platform;

        m_materialsDesc.emplace_back( desc );
    }
    //////////////////////////////////////////////////////////////////////////
    void Package::addPackageSetting( const ConstString & _name, const FilePath & _filePath, const Tags & _platform )
    {
        ContentInterfacePtr content = Helper::makeFileContent( m_fileGroup, _filePath, MENGINE_DOCUMENT_FACTORABLE );

        PackageSettingDesc desc;
        desc.name = _name;
        desc.content = content;
        desc.platform = _platform;

        m_settingsDesc.emplace_back( desc );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Package::enableResources_( const VectorConstString & _locales, const PackageResourceDesc & _desc )
    {
        Metacode::Meta_Data::Meta_DataBlock datablock;

        bool exist = false;
        if( LOADER_SERVICE()
            ->load( _desc.content, &datablock, Metacode::Meta_Data::getVersion(), &exist, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            if( exist == false )
            {
                LOGGER_ERROR( "resource '%s' not found"
                    , Helper::getContentFullPath( _desc.content ).c_str()
                );
            }
            else
            {
                LOGGER_ERROR( "invalid parse resource '%s'"
                    , Helper::getContentFullPath( _desc.content ).c_str()
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
                    , Helper::getContentFullPath( _desc.content ).c_str()
                    , name.c_str()
                    , groupName.c_str()
                    , has_resource->getGroupName().c_str()
                    , has_resource->getName().c_str()
                );

                return false;
            }

            ResourceCook cook;
            cook.locales = _locales;
            cook.groupName = groupName;
            cook.name = name;
            cook.type = type;
            cook.groupCache = true;
            cook.keep = true;

            ResourcePtr resource = RESOURCE_SERVICE()
                ->createResource( cook, MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( resource, "file '%s' group '%s' resource '%s' type '%s' invalid create"
                , Helper::getContentFullPath( _desc.content ).c_str()
                , groupName.c_str()
                , name.c_str()
                , type.c_str()
            );

            resource->setTags( _desc.tags );

            ContentInterfacePtr content = Helper::makeFileContent( m_fileGroup, FilePath::none(), MENGINE_DOCUMENT_FACTORABLE );

            resource->setContent( content );
            
            MetabufLoaderInterfacePtr loader = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "MetabufLoader" ), type );

            MENGINE_ASSERTION_MEMORY_PANIC( loader, "file '%s' group '%s' resource '%s' type '%s' invalid create loader"
                , Helper::getContentFullPath( _desc.content ).c_str()
                , groupName.c_str()
                , resource->getName().c_str()
                , resource->getType().c_str()
            );

            if( loader->load( resource, meta_resource ) == false )
            {
                LOGGER_ERROR( "file '%s' group '%s' resource '%s' type '%s' invalid load"
                    , Helper::getContentFullPath( _desc.content ).c_str()
                    , groupName.c_str()
                    , resource->getName().c_str()
                    , resource->getType().c_str()
                );

                return false;
            }

            resource->setIgnored( _desc.ignored );

            if( resource->initialize() == false )
            {
                LOGGER_ERROR( "file '%s' group '%s' resource '%s' type '%s' invalid initialize"
                    , Helper::getContentFullPath( _desc.content ).c_str()
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

            ContentInterfacePtr content = Helper::makeFileContent( m_fileGroup, path, MENGINE_DOCUMENT_FACTORABLE );

            PackageResourceDesc desc;
            desc.content = content;
            desc.tags = _desc.tags;
            desc.platform = _desc.platform;
            desc.demand = _desc.demand;
            desc.ignored = _desc.ignored;

            if( this->enableResources_( _locales, desc ) == false )
            {
                LOGGER_ERROR( "load '%s' resource invalid load include '%s'"
                    , Helper::getContentFullPath( _desc.content ).c_str()
                    , path.c_str()
                );

                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Package::disableResources_( const VectorConstString & _locales, const PackageResourceDesc & _desc )
    {
        Metacode::Meta_Data::Meta_DataBlock datablock;

        bool exist = false;
        if( LOADER_SERVICE()
            ->load( _desc.content, &datablock, Metacode::Meta_Data::getVersion(), &exist, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            if( exist == false )
            {
                LOGGER_ERROR( "file resources '%s' not found"
                    , Helper::getContentFullPath( _desc.content ).c_str()
                );
            }
            else
            {
                LOGGER_ERROR( "invalid parse resources '%s'"
                    , Helper::getContentFullPath( _desc.content ).c_str()
                );
            }

            return false;
        }

        const ConstString & groupName = datablock.get_Name();

        const Metacode::Meta_Data::Meta_DataBlock::VectorMeta_Include & includes_include = datablock.get_Includes_Include();

        for( const Metacode::Meta_Data::Meta_DataBlock::Meta_Include & meta_include : includes_include )
        {
            const FilePath & path = meta_include.get_Path();

            ContentInterfacePtr content = Helper::makeFileContent( m_fileGroup, path, MENGINE_DOCUMENT_FACTORABLE );

            PackageResourceDesc desc;
            desc.content = content;
            desc.tags = _desc.tags;
            desc.platform = _desc.platform;
            desc.demand = _desc.demand;
            desc.ignored = _desc.ignored;

            if( this->disableResources_( _locales, _desc ) == false )
            {
                LOGGER_ERROR( "load '%s' resource invalid load include '%s'"
                    , Helper::getContentFullPath( _desc.content ).c_str()
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
                    , Helper::getContentFullPath( _desc.content ).c_str()
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
    bool Package::enableMaterials_( const PackageMaterialDesc & _desc )
    {
        Metacode::Meta_Data::Meta_DataBlock datablock;

        bool exist = false;
        if( LOADER_SERVICE()
            ->load( _desc.content, &datablock, Metacode::Meta_Data::getVersion(), &exist, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            if( exist == false )
            {
                LOGGER_ERROR( "materials '%s' not found"
                    , Helper::getContentFullPath( _desc.content ).c_str()
                );
            }
            else
            {
                LOGGER_ERROR( "invalid parse materials '%s'"
                    , Helper::getContentFullPath( _desc.content ).c_str()
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

            ContentInterfacePtr fragmentShaderContent = Helper::makeFileContent( m_fileGroup, fragmentShaderFilePath, MENGINE_DOCUMENT_FACTORABLE );
            
            fragmentShaderContent->setConverterType( fileConverterType );

            RenderFragmentShaderInterfacePtr shader = RENDERMATERIAL_SERVICE()
                ->createFragmentShader( name, fragmentShaderContent, isCompile, MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( shader, "material '%s' invalid load '%s' fragment shader '%s' compile [%u]"
                , Helper::getContentFullPath( _desc.content ).c_str()
                , Helper::getContentFullPath( fragmentShaderContent ).c_str()
                , name.c_str()
                , isCompile
            );

            MENGINE_UNUSED( shader );
        }

        const Metacode::Meta_Data::Meta_DataBlock::VectorMeta_VertexShader & includes_VertexShader = datablock.get_Includes_VertexShader();

        for( const Metacode::Meta_Data::Meta_DataBlock::Meta_VertexShader & meta_VertexShader : includes_VertexShader )
        {
            const ConstString & name = meta_VertexShader.get_Name();

            const ConstString & renderPlatform = meta_VertexShader.get_RenderPlatform();

            if( renderPlatform != renderPlatformName )
            {
                continue;
            }            

            const FilePath & vertexShaderFilePath = meta_VertexShader.get_File_Path();

            ConstString fileConverterType;
            meta_VertexShader.get_File_Converter( &fileConverterType );

            bool isCompile = false;
            meta_VertexShader.get_File_Compile( &isCompile );

            ContentInterfacePtr vertexShaderContent = Helper::makeFileContent( m_fileGroup, vertexShaderFilePath, MENGINE_DOCUMENT_FACTORABLE );

            vertexShaderContent->setConverterType( fileConverterType );

            RenderVertexShaderInterfacePtr shader = RENDERMATERIAL_SERVICE()
                ->createVertexShader( name, vertexShaderContent, isCompile, MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( shader, "material '%s' invalid load '%s' vertex shader '%s' compile [%u]"
                , Helper::getContentFullPath( _desc.content ).c_str()
                , Helper::getContentFullPath( vertexShaderContent ).c_str()
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
                , Helper::getContentFullPath( _desc.content ).c_str()
                , name.c_str()
                , vertexShaderName.c_str()
            );

            const RenderFragmentShaderInterfacePtr & fragmentShader = RENDERMATERIAL_SERVICE()
                ->getFragmentShader( fragmentShaderName );

            MENGINE_ASSERTION_MEMORY_PANIC( fragmentShader, "material '%s' program '%s' not found fragment shader '%s'"
                , Helper::getContentFullPath( _desc.content ).c_str()
                , name.c_str()
                , fragmentShaderName.c_str()
            );

            const RenderVertexAttributeInterfacePtr & vertexAttribute = RENDERMATERIAL_SERVICE()
                ->getVertexAttribute( vertexAttributeName );

            MENGINE_ASSERTION_MEMORY_PANIC( vertexAttribute, "material '%s' program '%s' not found vertex attribute '%s'"
                , Helper::getContentFullPath( _desc.content ).c_str()
                , name.c_str()
                , vertexAttributeName.c_str()
            );

            RenderProgramInterfacePtr program = RENDERMATERIAL_SERVICE()
                ->createProgram( name, vertexShader, fragmentShader, vertexAttribute, samplerCount, MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( program, "material '%s' invalid create program vertex '%s' fragment '%s'"
                , Helper::getContentFullPath( _desc.content ).c_str()
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
                    , Helper::getContentFullPath( _desc.content ).c_str()
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
                , Helper::getContentFullPath( _desc.content ).c_str()
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
    bool Package::disableMaterials_( const PackageMaterialDesc & _desc )
    {
        Metacode::Meta_Data::Meta_DataBlock datablock;

        bool exist = false;
        if( LOADER_SERVICE()
            ->load( _desc.content, &datablock, Metacode::Meta_Data::getVersion(), &exist, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            if( exist == false )
            {
                LOGGER_ERROR( "materials '%s' not found"
                    , Helper::getContentFullPath( _desc.content ).c_str()
                );
            }
            else
            {
                LOGGER_ERROR( "invalid parse materials '%s'"
                    , Helper::getContentFullPath( _desc.content ).c_str()
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