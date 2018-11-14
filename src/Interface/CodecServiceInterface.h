#pragma once

#include "Config/Typedef.h"

#include "Interface/ServiceInterface.h"
#include "Interface/DecoderFactoryInterface.h"
#include "Interface/EncoderFactoryInterface.h"

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
        virtual bool registerDecoder( const ConstString& _type, const DecoderFactoryInterfacePtr & _decoder ) = 0;
        virtual DecoderFactoryInterfacePtr unregisterDecoder( const ConstString& _type ) = 0;

        virtual bool registerEncoder( const ConstString& _type, const EncoderFactoryInterfacePtr & _encoder ) = 0;
        virtual EncoderFactoryInterfacePtr unregisterEncoder( const ConstString& _type ) = 0;

    public:
        virtual DecoderInterfacePtr createDecoder( const ConstString & _type ) = 0;

        template<class T>
        T createDecoderT( const ConstString& _type )
        {
            DecoderInterfacePtr decoder = this->createDecoder( _type );

#ifndef NDEBUG
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
        virtual EncoderInterfacePtr createEncoder( const ConstString& _type ) = 0;

        template<class T>
        T createEncoderT( const ConstString& _type )
        {
            EncoderInterfacePtr encoder = this->createEncoder( _type );

#ifndef NDEBUG
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
        virtual bool registerCodecExt( const String & _ext, const ConstString & _codecType ) = 0;
        virtual const ConstString & findCodecType( const FilePath & _path ) const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define CODEC_SERVICE()\
    ((Mengine::CodecServiceInterface *)SERVICE_GET(Mengine::CodecServiceInterface))
//////////////////////////////////////////////////////////////////////////

