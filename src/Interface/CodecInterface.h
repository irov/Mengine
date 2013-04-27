#	pragma once

#	include "Config/Typedef.h"

#	include "Interface/PluginInterface.h"
#	include "Interface/StreamInterface.h"
#	include "Interface/FileSystemInterface.h"

#   include "Utils/Factory/FactorablePtr.h"
#   include "Utils/Core/IntrusivePtr.h"

#	include "Utils/Core/FilePath.h"

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
        : public FactorablePtr
	{
    public:
        virtual bool initialize( ServiceProviderInterface * _serviceProvider, const InputStreamInterfacePtr & _stream ) = 0;

    public:
        virtual bool setOptions( CodecOptions * _options ) = 0;

	public:
        virtual InputStreamInterfacePtr getStream() const = 0;
		virtual const CodecDataInfo* getCodecDataInfo() const = 0;		

    public:
		virtual unsigned int decode( unsigned char* _buffer, unsigned int _bufferSize ) = 0;
	};

    typedef IntrusivePtr<DecoderInterface> DecoderInterfacePtr;

	class DecoderFactoryInterface
		: public CodecFactoryInterface
	{
	public:
		virtual DecoderInterfacePtr createDecoder() = 0;
        virtual const ConstString & getName() const = 0;
	};

	class EncoderInterface
        : public FactorablePtr
	{
    public:
        virtual bool initialize( ServiceProviderInterface * _serviceProvider, const OutputStreamInterfacePtr & _stream ) = 0;

    public:
        virtual bool setOptions( CodecOptions * _options ) = 0;

	public:
		virtual unsigned int encode( unsigned char* _buffer, const CodecDataInfo* _bufferDataInfo ) = 0;

    public:
        virtual OutputStreamInterfacePtr getStream() const = 0;
	};

    typedef IntrusivePtr<EncoderInterface> EncoderInterfacePtr;

	class EncoderFactoryInterface
		: public CodecFactoryInterface
	{
	public:
		virtual EncoderInterfacePtr createEncoder() = 0;
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
		virtual DecoderInterfacePtr createDecoder( const ConstString & _type, const InputStreamInterfacePtr & _stream ) = 0; 

        template<class T>
        T createDecoderT( const ConstString& _type, const InputStreamInterfacePtr & _stream )
        {
            DecoderInterfacePtr decoder = this->createDecoder( _type, _stream );

            return intrusive_dynamic_cast<T>(decoder);
        }

    public:
        virtual EncoderInterfacePtr createEncoder( const ConstString& _type, const OutputStreamInterfacePtr & stream ) = 0;

        template<class T>
        T createEncoderT( const ConstString& _type, const OutputStreamInterfacePtr & _stream )
        {
            EncoderInterfacePtr encoder = this->createEncoder( _type, _stream );

            return intrusive_dynamic_cast<T>(encoder);
        }

	public:
		virtual bool registerCodecExt( const String & _ext, const ConstString & _codecType ) = 0;
		virtual const ConstString & findCodecType( const FilePath & _path ) const = 0;
	};

#   define CODEC_SERVICE( serviceProvider )\
    (Menge::getService<Menge::CodecServiceInterface>(serviceProvider))
}

