#	pragma once

#	include "Config/Typedef.h"

#	include "Interface/PluginInterface.h"
#	include "Interface/StreamInterface.h"
#	include "Interface/FileSystemInterface.h"

#	include "Core/FilePath.h"

namespace Menge
{
	struct CodecDataInfo 
	{
		size_t size;
	};

	struct CodecOptions
	{
	};

	class CodecServiceInterface;

	class CodecFactoryInterface
	{
	public:
        virtual void destroy() = 0;
	};

	class DecoderInterface
        : public Factorable
	{
    public:
        virtual bool initialize( ServiceProviderInterface * _serviceProvider, InputStreamInterface * _stream ) = 0;

    public:
        virtual bool setOptions( CodecOptions * _options ) = 0;

	public:
        virtual InputStreamInterface * getStream() const = 0;
		virtual const CodecDataInfo* getCodecDataInfo() const = 0;		

    public:
		virtual unsigned int decode( unsigned char* _buffer, unsigned int _bufferSize ) = 0;
	};

	class DecoderFactoryInterface
		: public CodecFactoryInterface
	{
	public:
		virtual DecoderInterface * createDecoder() = 0;
        virtual const ConstString & getName() const = 0;
	};

	class EncoderInterface
        : public Factorable
	{
    public:
        virtual bool initialize( ServiceProviderInterface * _serviceProvider, OutputStreamInterface * _stream ) = 0;

    public:
        virtual bool setOptions( CodecOptions * _options ) = 0;

	public:
		virtual unsigned int encode( unsigned char* _buffer, const CodecDataInfo* _bufferDataInfo ) = 0;

    public:
        virtual OutputStreamInterface * getStream() const = 0;
	};

	class EncoderFactoryInterface
		: public CodecFactoryInterface
	{
	public:
		virtual EncoderInterface * createEncoder() = 0;
        virtual const ConstString & getName() const = 0;
	};

	enum ECodecTargetFormat
	{
		ECTF_Texture
	};

	class CodecServiceInterface
		: public ServiceInterface
	{
        SERVICE_DECLARE("CodecService")    

	public:
		virtual void registerDecoder( const ConstString& _type, DecoderFactoryInterface * _interface ) = 0;
		virtual void unregisterDecoder( const ConstString& _type ) = 0;
		
		virtual void registerEncoder( const ConstString& _type, EncoderFactoryInterface * _interface ) = 0;
		virtual void unregisterEncoder( const ConstString& _type ) = 0;

    public:
		virtual DecoderInterface * createDecoder( const ConstString & _type, InputStreamInterface * _stream ) = 0; 

        template<class T>
        T * createDecoderT( const ConstString& _type, InputStreamInterface * _stream )
        {
            DecoderInterface * decoder = this->createDecoder( _type, _stream );

            return dynamic_cast<T*>(decoder);
        }

    public:
        virtual EncoderInterface * createEncoder( const ConstString& _type, OutputStreamInterface * stream ) = 0;

        template<class T>
        T * createEncoderT( const ConstString& _type, OutputStreamInterface * _stream )
        {
            EncoderInterface * encoder = this->createEncoder( _type, _stream );

            return dynamic_cast<T*>(encoder);
        }

	public:
		virtual bool registerCodecExt( const String & _ext, const ConstString & _codecType ) = 0;
		virtual const ConstString & findCodecType( const FilePath & _path ) const = 0;
	};

#   define CODEC_SERVICE( serviceProvider )\
    (Menge::getService<Menge::CodecServiceInterface>(serviceProvider))
}

