#include "MetabufPackageLoader.h"

#include "Interface/LoaderServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/ContentHelper.h"

#include "Metacode/Metacode.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MetabufPackageLoader::MetabufPackageLoader()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MetabufPackageLoader::~MetabufPackageLoader()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetabufPackageLoader::load( const PackageInterfacePtr & _package, const DocumentInterfacePtr & _doc )
    {
        const FileGroupInterfacePtr &  fileGroup = _package->getFileGroup();
        const PackageDesc & desc = _package->getPackageDesc();

        ContentInterfacePtr descriptionContent = Helper::makeFileContent( fileGroup, desc.descriptionPath, _doc );

        Metacode::Meta_Data::Meta_Pak pak;

        bool exist = false;
        if( LOADER_SERVICE()
            ->load( descriptionContent, &pak, Metacode::Meta_Data::getVersion(), &exist, _doc ) == false )
        {
            LOGGER_ERROR( "invalid resource file '%s' name '%s' description '%s'"
                , desc.path.c_str()
                , desc.name.c_str()
                , Helper::getContentFullPath( descriptionContent )
            );

            return false;
        }

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

                _package->addPackageScript( Path, Module, Initializer, Finalizer, Platform );
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

                _package->addPackageFontPath( Path, Platform );
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

                _package->addPackageResource( Path, tags, platform, demand, ignored );
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

                _package->addPackageTextPath( Path, platform );
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

                _package->addPackageData( name, path, platform );
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

                _package->addPackageMaterial( path, platform );
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

                _package->addPackageSetting( name, path, platform );
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}