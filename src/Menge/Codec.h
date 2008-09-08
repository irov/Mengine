#	pragma once

#	include "Config/Typedef.h"
#	include "Interface/FileSystemInterface.h"

#	include <map>

namespace Menge 
{
	class Codec
	{
	public:
		class CodecData 
		{
		public:
			virtual ~CodecData() {};
		};

	public:
		virtual ~Codec();

		virtual bool code( OutStreamInterface* _outStream, unsigned char* _buffer, CodecData* _data ) const = 0;
		virtual bool getDataInfo( DataStreamInterface* _inputData, CodecData* _codecData ) const = 0;
		virtual bool decode( DataStreamInterface* _input, unsigned char* _buffer, unsigned int _options ) const = 0;
	};

	class CodecManager
	{
	public:
		static void registerCodec( const StringA& _type, Codec* _codec );
		static void unregisterCodec( const StringA& _type );

		static void initialize();
		static void cleanup();
	
		static Codec* getCodec( const StringA& _type );

	protected:
		typedef std::map< StringA, Codec* > TCodecMap; 
		static TCodecMap ms_mapCodecs;
	};
} // namespace Menge
