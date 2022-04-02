#include "ResourceConvertPlugin.h"

#include "Interface/FileServiceInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/ConverterServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"
#include "Interface/CodecServiceInterface.h"

#include "Kernel/AssertionVocabulary.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/NotificationHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( ResourceConvertService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( ResourceConvert, Mengine::ResourceConvertPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceConvertPlugin::ResourceConvertPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceConvertPlugin::~ResourceConvertPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceConvertPlugin::_availablePlugin() const
    {
        bool Engine_ResourceConvertPluginAvailable = CONFIG_VALUE( "Engine", "ResourceConvertPluginAvailable", true );

        if( Engine_ResourceConvertPluginAvailable == false )
        {
            return false;
        }

        bool OPTION_noconvert = HAS_OPTION( "noconvert" );

        if( OPTION_noconvert == true )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceConvertPlugin::_initializePlugin()
    {
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_DEVELOPMENT_RESOURCE_INITIALIZE, &ResourceConvertPlugin::convertResource_, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceConvertPlugin::_finalizePlugin()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_DEVELOPMENT_RESOURCE_INITIALIZE );

        MENGINE_ASSERTION_VOCABULARY_EMPTY( STRINGIZE_STRING_LOCAL( "Converter" ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceConvertPlugin::convertResource_( Resource * _resource )
    {
        const ContentInterfacePtr & content = _resource->getContent();

        if( content == nullptr )
        {
            return;
        }

        const ConstString & converterType = content->getConverterType();

        if( converterType.empty() == true )
        {
            return;
        }

        LOGGER_INFO( "convert", "convert resource '%s' type '%s' group '%s' file locale '%s' converter '%s'"
            , _resource->getName().c_str()
            , _resource->getType().c_str()
            , _resource->getGroupName().c_str()
            , _resource->getLocale().c_str()
            , content->getConverterType().c_str()
        );

        const FileGroupInterfacePtr & fileGroup = content->getFileGroup();
        const FilePath & filePath = content->getFilePath();

        FilePath newFilePath;
        if( CONVERTER_SERVICE()
            ->convert( converterType, fileGroup, filePath, &newFilePath, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            LOGGER_ERROR( "resource '%s' group '%s' can't convert '%s':'%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , content->getFilePath().c_str()
                , content->getConverterType().c_str()
            );

            throw ExceptionNotificationFailed();
        }

        content->setFilePath( newFilePath );

        const ConstString & codecType = CODEC_SERVICE()
            ->findCodecType( newFilePath );

        content->setCodecType( codecType );

        DataflowInterfacePtr dataflow = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Dataflow" ), codecType );

        content->setDataflow( dataflow );
    }
    //////////////////////////////////////////////////////////////////////////
}
