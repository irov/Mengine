#pragma once

#include "Config/Typedef.h"

#include "Interface/ServiceInterface.h"
#include "Interface/DecoderFactoryInterface.h"
#include "Interface/EncoderFactoryInterface.h"

#include "Kernel/DocumentHelper.h"
#include "Kernel/Factorable.h"
#include "Kernel/FilePath.h"

#ifdef MENGINE_DEBUG
#   include <type_traits>
#   include <stdexcept>
#endif

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
            static_assert(std::is_base_of_v<DecoderInterface, std::remove_pointer_t<typename T::value_type>>, "static decoder cast use on non 'DecoderInterface' type");

            if( decoder == nullptr )
            {
                return nullptr;
            }

            if( stdex::intrusive_dynamic_cast<T>(decoder) == nullptr )
            {
                throw std::runtime_error( "static decoder cast" );
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
            static_assert(std::is_base_of_v<EncoderInterface, std::remove_pointer_t<typename T::value_type>>, "static encoder cast use on non 'EncoderInterface' type");

            if( encoder == nullptr )
            {
                return nullptr;
            }

            if( stdex::intrusive_dynamic_cast<T>(encoder) == nullptr )
            {
                throw std::runtime_error( "static encoder cast" );
            }
#endif

            T t = stdex::intrusive_static_cast<T>(encoder);

            return t;
        }

    public:
        virtual bool registerCodecExt( const ConstString & _ext, const ConstString & _codecType ) = 0;
        virtual void removeCodecExt( const ConstString & _ext ) = 0;
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
#define CODEC_UNREGISTER_EXT(EXT)\
    CODEC_SERVICE()->removeCodecExt( EXT )
//////////////////////////////////////////////////////////////////////////