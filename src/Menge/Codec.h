#	pragma once

#	include <map>

#	include "Config/Typedef.h"
#	include "Interface/FileSystemInterface.h"

namespace Menge 
{
	//Abstract class that defines a 'codec'.
	//@remarks
	//A codec class works like a two-way filter for data - data entered on
	//one end (the decode end) gets processed and transformed into easily
	//usable data while data passed the other way around codes it back.
	//@par
	//The codec concept is a pretty generic one - you can easily understand
	//how it can be used for images, sounds, archives, even compressed data.
	class Codec
	{
	protected:
		typedef std::map< String, Codec* > TCodecMap; 
		// A map that contains all the registered codecs.
		static TCodecMap ms_mapCodecs;

	public:
		class CodecData 
		{
		public:
			virtual ~CodecData() {};
		};

	public:
		virtual ~Codec();

		// Registers a new codec in the database.
		static void registerCodec( Codec *_codec )
		{
			ms_mapCodecs[_codec->getType()] = _codec;
		}

		// Unregisters a codec from the database.
		static void unRegisterCodec( Codec *pCodec )
		{
			ms_mapCodecs.erase(pCodec->getType());
		}

		static void initialize();
		static void cleanup();
		// Gets the file extension list for the registered codecs.
		static TStringVector getExtensions();

		// Gets the codec registered for the passed in file extension.
		static Codec* getCodec( const String& _extension );

		// Codes the data in the input stream and saves the result in the output stream.
		virtual DataStreamInterface* code( DataStreamInterface* input, CodecData* _data ) const = 0;

		//Codes the data in the input chunk and saves the result in the output
		//filename provided. Provided for efficiency since coding to memory is
		//progressive therefore memory required is unknown leading to reallocations.
		//@param input The input data
		//@param outFileName The filename to write to
		//@param pData Extra information to be passed to the codec (codec type specific)
		virtual void codeToFile( DataStreamInterface* _input, const String& _outFileName, CodecData* _pData ) const = 0;

		/// Result of a decoding; both a decoded data stream and CodecData metadata
		typedef std::pair<DataStreamInterface*, CodecData*> DecodeResult;

		//Codes the data from the input chunk into the output chunk.
		//@param input Stream containing the encoded data
		//@note
		//Has a variable number of arguments, which depend on the codec type.
		virtual DecodeResult decode( DataStreamInterface* _input ) const = 0;

		virtual bool getDataInfo( DataStreamInterface* _inputData, CodecData* _codecData ) const = 0;
		virtual bool decode( DataStreamInterface* _input, unsigned char* _buffer, unsigned int _options ) const = 0;
		// Returns the type of the codec as a String
		virtual String getType() const = 0;
	};
} // namespace Menge
