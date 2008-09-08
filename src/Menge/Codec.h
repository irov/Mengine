#	pragma once

#	include "Config/Typedef.h"
#	include "Interface/FileSystemInterface.h"

#	include <map>

namespace Menge 
{
	class Codec
	{
	protected:
		typedef std::map< String, Codec* > TCodecMap; 
		static TCodecMap ms_mapCodecs;
	public:

		class CodecData 
		{
		public:
			virtual ~CodecData() {};
		};

	public:
		virtual ~Codec();

		static void registerCodec( const String& _type, Codec* _codec );
		static void unregisterCodec( const String& _type );

		static void initialize();
		static void cleanup();
	
		static Codec* getCodec( const String& _extension );
		// Codes the data in the input stream and saves the result in the output stream.
		//virtual DataStreamInterface* code( DataStreamInterface* input, CodecData* _data ) const = 0;
		virtual bool code( OutStreamInterface* _outStream, unsigned char* _buffer, CodecData* _data ) const = 0;
		virtual bool getDataInfo( DataStreamInterface* _inputData, CodecData* _codecData ) const = 0;
		virtual bool decode( DataStreamInterface* _input, unsigned char* _buffer, unsigned int _options ) const = 0;
	};
} // namespace Menge
