#include "Package.h"

#include "Interface/FileServiceInterface.h"
#include "Interface/TextServiceInterface.h"
#include "Interface/FontServiceInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/UserdataServiceInterface.h"
#include "Interface/PlatformServiceInterface.h"
#include "Interface/RenderSystemInterface.h"
#include "Interface/SettingsServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/Resource.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/FileGroupHelper.h"
#include "Kernel/ContentHelper.h"
#include "Kernel/VocabularyHelper.h"
#include "Kernel/FilePathHelper.h"

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

            if( desc.renderPlatform.empty() == false )
            {
                if( SERVICE_IS_INITIALIZE( RenderSystemInterface ) == false )
                {
                    LOGGER_ERROR( "file '%s' package '%s' invalid load material '%s' render platform '%s' without RenderSystem"
                        , m_desc.path.c_str()
                        , m_desc.name.c_str()
                        , Helper::getContentFullPath( desc.content ).c_str()
                        , desc.renderPlatform.c_str()
                    );

                    return false;
                }

                const ConstString & renderPlatformName = RENDER_SYSTEM()
                    ->getRenderPlatformName();

                if( desc.renderPlatform != renderPlatformName )
                {
                    continue;
                }
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

            if( desc.renderPlatform.empty() == false )
            {
                if( SERVICE_IS_INITIALIZE( RenderSystemInterface ) == false )
                {
                    LOGGER_ERROR( "file '%s' package '%s' invalid unload material '%s' render platform '%s' without RenderSystem"
                        , m_desc.path.c_str()
                        , m_desc.name.c_str()
                        , Helper::getContentFullPath( desc.content ).c_str()
                        , desc.renderPlatform.c_str()
                    );

                    return false;
                }

                const ConstString & renderPlatformName = RENDER_SYSTEM()
                    ->getRenderPlatformName();

                if( desc.renderPlatform != renderPlatformName )
                {
                    continue;
                }
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
        this->addPackageMaterial( _filePath, _platform, ConstString::none() );
    }
    //////////////////////////////////////////////////////////////////////////
    void Package::addPackageMaterial( const FilePath & _filePath, const Tags & _platform, const ConstString & _renderPlatform )
    {
        ContentInterfacePtr content = Helper::makeFileContent( m_fileGroup, _filePath, MENGINE_DOCUMENT_FACTORABLE );

        PackageMaterialDesc desc;
        desc.content = content;
        desc.platform = _platform;
        desc.renderPlatform = _renderPlatform;

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
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static PackageContentLoaderInterfacePtr getPackageContentLoader( const ContentInterfacePtr & _content, const Char * _kind )
        {
            const FilePath & filePath = _content->getFilePath();
            ConstString ext = Helper::getFilePathExt( filePath );

            PackageContentLoaderInterfacePtr loader = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "PackageContentLoader" ), ext );

            if( loader == nullptr )
            {
                LOGGER_ERROR( "no PackageContentLoader for %s DataBlock '%s' extension '%s'"
                    , _kind
                    , Helper::getContentFullPath( _content ).c_str()
                    , ext.c_str()
                );
            }

            return loader;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    bool Package::enableResources_( const VectorConstString & _locales, const PackageResourceDesc & _desc )
    {
        PackageContentLoaderInterfacePtr loader = Detail::getPackageContentLoader( _desc.content, "resource" );

        if( loader == nullptr )
        {
            return false;
        }

        return loader->enableResources( PackageInterfacePtr::from( this ), _locales, _desc );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Package::disableResources_( const VectorConstString & _locales, const PackageResourceDesc & _desc )
    {
        PackageContentLoaderInterfacePtr loader = Detail::getPackageContentLoader( _desc.content, "resource" );

        if( loader == nullptr )
        {
            return false;
        }

        return loader->disableResources( PackageInterfacePtr::from( this ), _locales, _desc );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Package::enableMaterials_( const PackageMaterialDesc & _desc )
    {
        PackageContentLoaderInterfacePtr loader = Detail::getPackageContentLoader( _desc.content, "material" );

        if( loader == nullptr )
        {
            return false;
        }

        return loader->enableMaterials( PackageInterfacePtr::from( this ), _desc );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Package::disableMaterials_( const PackageMaterialDesc & _desc )
    {
        PackageContentLoaderInterfacePtr loader = Detail::getPackageContentLoader( _desc.content, "material" );

        if( loader == nullptr )
        {
            return false;
        }

        return loader->disableMaterials( PackageInterfacePtr::from( this ), _desc );
    }
    //////////////////////////////////////////////////////////////////////////
}
