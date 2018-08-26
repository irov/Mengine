#pragma once

#include "Config/Typedef.h"

#include "Interface/PluginInterface.h"
#include "Interface/StreamInterface.h"

#include "Kernel/Factorable.h"

#include "Kernel/FilePath.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct CodecDataInfo
    {
    };
    //////////////////////////////////////////////////////////////////////////
    struct CodecOptions
    {
    };
    //////////////////////////////////////////////////////////////////////////
    class CodecFactoryInterface
        : public ServantInterface
    {
    public:
        virtual bool initialize() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<CodecFactoryInterface> CodecFactoryInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class DecoderInterface
        : public ServantInterface
    {
    public:
        virtual bool initialize() = 0;
        virtual void finalize() = 0;

    public:
        virtual bool setOptions( const CodecOptions * _options ) = 0;

    public:
        virtual const InputStreamInterfacePtr & getStream() const = 0;

    public:
        virtual void setCodecDataInfo( const CodecDataInfo * _dataInfo ) = 0;
        virtual const CodecDataInfo * getCodecDataInfo() const = 0;

    public:
        virtual bool prepareData( const InputStreamInterfacePtr & _stream ) = 0;

    public:
        virtual size_t decode( void * _buffer, size_t _bufferSize ) = 0;
        virtual bool rewind() = 0;

    public:
        virtual bool seek( float _time ) = 0;
        virtual float tell() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DecoderInterface> DecoderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class DecoderFactoryInterface
        : public CodecFactoryInterface
    {
    public:
        virtual DecoderInterfacePtr createDecoder() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DecoderFactoryInterface> DecoderFactoryInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class EncoderInterface
        : public ServantInterface
    {
    public:
        virtual bool initialize( const OutputStreamInterfacePtr & _stream ) = 0;
        virtual void finalize() = 0;

    public:
        virtual bool setOptions( CodecOptions * _options ) = 0;

    public:
        virtual size_t encode( const void * _buffer, size_t _size, const CodecDataInfo* _dataInfo ) = 0;

    public:
        virtual OutputStreamInterfacePtr getStream() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<EncoderInterface> EncoderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class EncoderFactoryInterface
        : public CodecFactoryInterface
    {
    public:
        virtual EncoderInterfacePtr createEncoder() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<EncoderFactoryInterface> EncoderFactoryInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    enum ECodecTargetFormat
    {
        ECTF_Texture
    };
    //////////////////////////////////////////////////////////////////////////
    class CodecServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "CodecService" )

    public:
        virtual bool registerDecoder( const ConstString& _type, const DecoderFactoryInterfacePtr & _decoder ) = 0;
        virtual bool unregisterDecoder( const ConstString& _type ) = 0;

        virtual bool registerEncoder( const ConstString& _type, const EncoderFactoryInterfacePtr & _encoder ) = 0;
        virtual bool unregisterEncoder( const ConstString& _type ) = 0;

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
    //////////////////////////////////////////////////////////////////////////
#   define CODEC_SERVICE()\
    ((Mengine::CodecServiceInterface *)SERVICE_GET(Mengine::CodecServiceInterface))
}

