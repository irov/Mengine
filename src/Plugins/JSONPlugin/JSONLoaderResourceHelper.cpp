#include "JSONLoaderResourceHelper.h"

#include "Interface/CodecServiceInterface.h"
#include "Interface/ResourceBankInterface.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ContentHelper.h"
#include "Kernel/JSONDataHelper.h"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        bool setJSONContentFile( const ContentInterfacePtr & _content, const jpp::object & _json, bool _codecFromPath )
        {
            FilePath filePath;
            if( Helper::getJSONMemberFilePath( _json, "File", "Path", &filePath ) == true )
            {
                _content->setFilePath( filePath );
            }

            ConstString codecType;
            if( Helper::getJSONMemberConstString( _json, "File", "Codec", &codecType ) == true )
            {
                _content->setCodecType( codecType );
            }
            else if( _codecFromPath == true )
            {
                const FilePath & contentFilePath = _content->getFilePath();

                codecType = CODEC_SERVICE()
                    ->findCodecType( contentFilePath );

                _content->setCodecType( codecType );
            }

            ConstString converterType;
            if( Helper::getJSONMemberConstString( _json, "File", "Converter", &converterType ) == true )
            {
                _content->setConverterType( converterType );
            }

            bool noExist = false;
            if( Helper::getJSONMemberBool( _json, "File", "NoExist", &noExist ) == true )
            {
                _content->setValidNoExist( noExist );
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool getJSONMemberVec2Required( const jpp::object & _json, const Char * _node, const Char * _key, mt::vec2f * const _value )
        {
            if( Helper::getJSONMemberVec2f( _json, _node, _key, _value ) == false )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool getJSONResourceImage( Resource * _resource, const ConstString & _groupName, const ConstString & _resourceImageName, ResourceImagePtr * const _resourceImage )
        {
            ResourceBankInterface * resourceBank = _resource->getResourceBank();

            const ResourceImagePtr & resourceImage = resourceBank->getResource( _groupName, _resourceImageName );

            MENGINE_ASSERTION_MEMORY_PANIC( resourceImage, "'%s' group '%s' invalid get image resource '%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , _resourceImageName.c_str()
            );

            *_resourceImage = resourceImage;

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
