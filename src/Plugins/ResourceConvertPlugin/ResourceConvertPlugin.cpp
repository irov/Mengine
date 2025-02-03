#include "ResourceConvertPlugin.h"

#include "Interface/FileServiceInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/ConverterServiceInterface.h"
#include "Interface/CodecServiceInterface.h"

#include "Kernel/AssertionVocabulary.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/VocabularyHelper.h"
#include "Kernel/PluginHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( ResourceConvertService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( ResourceConvert, Mengine::ResourceConvertPlugin );
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
        bool OPTION_noconvert = HAS_OPTION( "noconvert" );

        if( OPTION_noconvert == true )
        {
            return false;
        }

        bool ResourceConvertPlugin_Available = CONFIG_VALUE_BOOLEAN( "ResourceConvertPlugin", "Available", true );

        if( ResourceConvertPlugin_Available == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceConvertPlugin::_initializePlugin()
    {
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_DEVELOPMENT_RESOURCE_INITIALIZE, &ResourceConvertPlugin::notifyDevelopmentResourceInitialize_, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceConvertPlugin::_finalizePlugin()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_DEVELOPMENT_RESOURCE_INITIALIZE );

        MENGINE_ASSERTION_VOCABULARY_EMPTY( STRINGIZE_STRING_LOCAL( "Converter" ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceConvertPlugin::notifyDevelopmentResourceInitialize_( Resource * _resource )
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

        //ToDo

        LOGGER_INFO( "convert", "convert resource '%s' type '%s' group '%s' file converter '%s'"
            , _resource->getName().c_str()
            , _resource->getType().c_str()
            , _resource->getGroupName().c_str()
            //, _resource->getLocale().c_str()
            , content->getConverterType().c_str()
        );

        ContentInterfacePtr newContent;
        if( CONVERTER_SERVICE()
            ->convert( converterType, content, &newContent, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            LOGGER_ERROR( "resource '%s' group '%s' can't file '%s' convert '%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , content->getFilePath().c_str()
                , content->getConverterType().c_str()
            );

            throw ExceptionNotificationFailed();
        }

        const FilePath & newFilePath = newContent->getFilePath();

        content->setFilePath( newFilePath );

        const ConstString & codecType = CODEC_SERVICE()
            ->findCodecType( newFilePath );

        content->setCodecType( codecType );

        DataflowInterfacePtr dataflow = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Dataflow" ), codecType );

        content->setDataflow( dataflow );
    }
    //////////////////////////////////////////////////////////////////////////
}
