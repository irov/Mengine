#	pragma once

#	include <map>

#	include "Config/Typedef.h"
#	include "Interface/FileSystemInterface.h"

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

		static void registerCodec( const String& _type, Codec *_codec )
		{
			ms_mapCodecs[_type] = _codec;
		}

		// Unregisters a codec from the database.
		/*static void unRegisterCodec( Codec *pCodec )
		{
			ms_mapCodecs.erase(pCodec->getType());
		}*/

		static void initialize();
		static void cleanup();
		// Gets the file extension list for the registered codecs.
		//static TStringVector getExtensions();

		// Gets the codec registered for the passed in file extension.
		static Codec* getCodec( const String& _extension );

		// Codes the data in the input stream and saves the result in the output stream.
		//virtual DataStreamInterface* code( DataStreamInterface* input, CodecData* _data ) const = 0;
		virtual bool code( OutStreamInterface* _outStream, unsigned char* _buffer, CodecData* _data ) const = 0;
		virtual bool getDataInfo( DataStreamInterface* _inputData, CodecData* _codecData ) const = 0;
		virtual bool decode( DataStreamInterface* _input, unsigned char* _buffer, unsigned int _options ) const = 0;
	};
} // namespace Menge
