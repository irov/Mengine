#pragma once

#include "Config/Typedef.h"

#include "Interface/ServiceInterface.h"
#include "Interface/DecoderFactoryInterface.h"
#include "Interface/EncoderFactoryInterface.h"

#include "Kernel/DocumentHelper.h"
#include "Kernel/Factorable.h"
#include "Kernel/FilePath.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class CodecServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "CodecService" )

    public:
        virtual DecoderInterfacePtr createDecoder( const ConstString & _type, const DocumentPtr & _doc ) = 0;

        template<class T>
        T createDecoderT( const ConstString & _type, const DocumentPtr & _doc )
        {
            DecoderInterfacePtr decoder = this->createDecoder( _type, _doc );

#ifdef MENGINE_DEBUG
            if( decoder == nullptr )
            {
                return nullptr;
            }

            if( stdex::intrusive_dynamic_cast<T>(decoder) == nullptr )
            {
                throw;
            }
#endif

            T t = stdex::intrusive_static_cast<T>(decoder);

            return t;
        }

    public:
        virtual EncoderInterfacePtr createEncoder( const ConstString & _type, const DocumentPtr & _doc ) = 0;

        template<class T>
        T createEncoderT( const ConstString & _type, const DocumentPtr & _doc )
        {
            EncoderInterfacePtr encoder = this->createEncoder( _type, _doc );

#ifdef MENGINE_DEBUG
            if( encoder == nullptr )
            {
                return nullptr;
            }

            if( stdex::intrusive_dynamic_cast<T>(encoder) == nullptr )
            {
                throw;
            }
#endif

            T t = stdex::intrusive_static_cast<T>(encoder);

            return t;
        }

    public:
        virtual bool registerCodecExt( const ConstString & _ext, const ConstString & _codecType ) = 0;
        virtual const ConstString & findCodecType( const FilePath & _filePath ) const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define CODEC_SERVICE()\
    ((Mengine::CodecServiceInterface *)SERVICE_GET(Mengine::CodecServiceInterface))
//////////////////////////////////////////////////////////////////////////
#define CODEC_REGISTER_EXT(EXT, TYPE)\
    CODEC_SERVICE()->registerCodecExt( EXT, TYPE )
//////////////////////////////////////////////////////////////////////////