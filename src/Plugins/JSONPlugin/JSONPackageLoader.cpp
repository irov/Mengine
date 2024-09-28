#include "JSONPackageLoader.h"

#include "JSONInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/ContentHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    JSONPackageLoader::JSONPackageLoader()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    JSONPackageLoader::~JSONPackageLoader()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONPackageLoader::load( const PackageInterfacePtr & _package, const DocumentInterfacePtr & _doc )
    {
        const FileGroupInterfacePtr &  fileGroup = _package->getFileGroup();
        const PackageDesc & desc = _package->getPackageDesc();

        ContentInterfacePtr content = Helper::makeFileContent( fileGroup, desc.descriptionPath, _doc );

        jpp::object json;
        if( JSON_SERVICE()
            ->load( content, &json, _doc ) == false )
        {
            LOGGER_ERROR( "invalid resource file '%s' name '%s' description '%s'"
                , desc.path.c_str()
                , desc.name.c_str()
                , desc.descriptionPath.c_str()
            );

            return false;
        }

        jpp::object json_Scripts = json.get( "Scripts" );

        if( json_Scripts.invalid() == false )
        {
            Tags platform = json_Scripts["Platform"];

            jpp::array json_Scripts_Script = json_Scripts["Script"];

            for( const jpp::object & item : json_Scripts_Script )
            {
                FilePath path = item["Path"];

                ConstString module = item["Module"];
                ConstString initializer = item["Initializer"];
                ConstString finalizer = item["Finalizer"];

                _package->addPackageScript( path, module, initializer, finalizer, platform );
            }
        }

        jpp::object json_Glyphs = json.get( "Glyphs" );

        if( json_Glyphs.invalid() == false )
        {
            Tags platform = json_Glyphs["Platform"];

            jpp::array json_Glyphs_Glyph = json_Glyphs["Glyph"];

            for( const jpp::object & item : json_Glyphs_Glyph )
            {
                FilePath path = item["Path"];

                _package->addPackageGlyphPath( path, platform );
            }
        }

        jpp::object json_Fonts = json.get( "Fonts" );

        if( json_Fonts.invalid() == false )
        {
            Tags platform = json_Fonts["Platform"];

            jpp::array json_Fonts_Font = json_Fonts["Font"];

            for( const jpp::object & item : json_Fonts_Font )
            {
                FilePath path = item["Path"];

                _package->addPackageFontPath( path, platform );
            }
        }

        jpp::object json_Resources = json.get( "Resources" );

        if( json_Resources.invalid() == false )
        {
            bool ignored = json_Resources.get( "Ignored", false );
            bool demand = json_Resources.get( "Demand", false );
            Tags platform = json_Resources["Platform"];

            jpp::array json_Resources_Resource = json_Resources["Resource"];

            for( const jpp::object & item : json_Resources_Resource )
            {
                FilePath path = item["Path"];
                Tags tags = item["Tags"];

                _package->addPackageResource( path, tags, platform, demand, ignored );
            }
        }

        jpp::object json_Texts = json.get( "Texts" );

        if( json_Texts.invalid() == false )
        {
            Tags platform = json_Texts["Platform"];

            jpp::array json_Texts_Text = json_Texts["Text"];

            for( const jpp::object & item : json_Texts_Text )
            {
                FilePath path = item["Path"];

                _package->addPackageTextPath( path, platform );
            }
        }

        jpp::object json_Datas = json.get( "Datas" );

        if( json_Datas.invalid() == false )
        {
            Tags platform = json_Datas["Platform"];

            jpp::array json_Datas_Data = json_Datas["Data"];

            for( const jpp::object & item : json_Datas_Data )
            {
                ConstString name = item["Name"];
                FilePath path = item["Path"];

                _package->addPackageData( name, path, platform );
            }
        }

        jpp::object json_Materials = json.get( "Materials" );

        if( json_Materials.invalid() == false )
        {
            Tags platform = json_Materials["Platform"];

            jpp::array json_Materials_Material = json_Materials["Material"];

            for( const jpp::object & item : json_Materials_Material )
            {
                FilePath path = item["Path"];

                _package->addPackageMaterial( path, platform );
            }
        }

        jpp::object json_Settings = json.get( "Settings" );

        if( json_Settings.invalid() == false )
        {
            Tags platform = json_Settings["Platform"];

            jpp::array json_Settings_Setting = json_Settings["Setting"];

            for( const jpp::object & item : json_Settings_Setting )
            {
                ConstString name = item["Name"];
                FilePath path = item["Path"];

                _package->addPackageSetting( name, path, platform );
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}