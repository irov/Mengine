#	pragma once

#	include "Config/Typedef.h"

#	include "Interface/PluginInterface.h"
#	include "Interface/StreamInterface.h"
#	include "Interface/FileSystemInterface.h"

#   include "Factory/FactorablePtr.h"

#	include "Core/FilePath.h"

#   include "stdex/intrusive_ptr.h"
#   include "stdex/intrusive_algorithm.h"

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
		: public FactorablePtr
	{
	};

	typedef stdex::intrusive_ptr<CodecFactoryInterface> CodecFactoryInterfacePtr;

	class DecoderInterface
        : public FactorablePtr
	{
	public:
		virtual void setServiceProvider( ServiceProviderInterface * _serviceProvider ) = 0;
		virtual ServiceProviderInterface * getServiceProvider() const = 0;

    public:
        virtual bool initialize( const InputStreamInterfacePtr & _stream ) = 0;
		virtual void finalize() = 0;

    public:
        virtual bool setOptions( CodecOptions * _options ) = 0;

	public:
        virtual const InputStreamInterfacePtr & getStream() const = 0;

	public:
		virtual void setCodecDataInfo( const CodecDataInfo * _dataInfo ) = 0;
		virtual const CodecDataInfo * getCodecDataInfo() const = 0;

	public:
		virtual size_t decode( void * _buffer, size_t _bufferSize ) = 0;
	};

    typedef stdex::intrusive_ptr<DecoderInterface> DecoderInterfacePtr;

	class DecoderFactoryInterface
		: public CodecFactoryInterface
	{
	public:
		virtual DecoderInterfacePtr createDecoder() = 0;
        virtual const ConstString & getName() const = 0;
	};

	typedef stdex::intrusive_ptr<DecoderFactoryInterface> DecoderFactoryInterfacePtr;

	class EncoderInterface
        : public FactorablePtr
	{
	public:
		virtual void setServiceProvider( ServiceProviderInterface * _serviceProvider ) = 0;
		virtual ServiceProviderInterface * getServiceProvider() const = 0;

    public:
        virtual bool initialize( const OutputStreamInterfacePtr & _stream ) = 0;
		virtual void finalize() = 0;

    public:
        virtual bool setOptions( CodecOptions * _options ) = 0;

	public:
		virtual size_t encode( const void * _buffer, const CodecDataInfo* _bufferDataInfo ) = 0;

    public:
        virtual OutputStreamInterfacePtr getStream() const = 0;
	};

    typedef stdex::intrusive_ptr<EncoderInterface> EncoderInterfacePtr;

	class EncoderFactoryInterface
		: public CodecFactoryInterface
	{
	public:
		virtual EncoderInterfacePtr createEncoder() = 0;
        virtual const ConstString & getName() const = 0;
	};

	typedef stdex::intrusive_ptr<EncoderFactoryInterface> EncoderFactoryInterfacePtr;

	enum ECodecTargetFormat
	{
		ECTF_Texture
	};

	class CodecServiceInterface
		: public ServiceInterface
	{
        SERVICE_DECLARE("CodecService")

	public:
		virtual void registerDecoder( const ConstString& _type, const DecoderFactoryInterfacePtr & _interface ) = 0;
		virtual void unregisterDecoder( const ConstString& _type ) = 0;
		
		virtual void registerEncoder( const ConstString& _type, const EncoderFactoryInterfacePtr & _interface ) = 0;
		virtual void unregisterEncoder( const ConstString& _type ) = 0;

    public:
		virtual DecoderInterfacePtr createDecoder( const ConstString & _type ) = 0; 

        template<class T>
        T createDecoderT( const ConstString& _type )
        {
            DecoderInterfacePtr decoder = this->createDecoder( _type );

#   ifdef _DEBUG
			if( stdex::intrusive_dynamic_cast<T>(decoder) == nullptr )
			{
				return nullptr;
			}
#   endif

            T t = stdex::intrusive_static_cast<T>(decoder);

            return t;
        }

    public:
        virtual EncoderInterfacePtr createEncoder( const ConstString& _type ) = 0;

        template<class T>
        T createEncoderT( const ConstString& _type )
        {
            EncoderInterfacePtr encoder = this->createEncoder( _type );

#   ifdef _DEBUG
            if( stdex::intrusive_dynamic_cast<T>(encoder) == nullptr )
            {
                return nullptr;
            }
#   endif

            T t = stdex::intrusive_static_cast<T>(encoder);

            return t;
        }

	public:
		virtual bool registerCodecExt( const String & _ext, const ConstString & _codecType ) = 0;
		virtual const ConstString & findCodecType( const FilePath & _path ) const = 0;
	};

#   define CODEC_SERVICE( serviceProvider )\
    (Menge::Helper::getService<Menge::CodecServiceInterface>(serviceProvider))
}

