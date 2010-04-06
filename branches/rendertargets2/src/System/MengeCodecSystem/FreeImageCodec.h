#	pragma once

#include "ImageCodec.h"
// Forward-declaration to avoid external dependency on FreeImage
struct FIBITMAP;

#	include <list>

namespace Menge
{
	//Codec specialized in images loaded using FreeImage.
	//@remarks
	//The users implementing subclasses of ImageCodec are required to return
	//a valid pointer to a ImageData class from the decode(...) function.
	class FreeImageCodec : public ImageCodec
	{
	public:
		FreeImageCodec( const String& _type, unsigned int _fiType );
		virtual ~FreeImageCodec() { }

		/// @copydoc Codec::code
		DataStreamInterface* code( DataStreamInterface* _input, CodecData* _data ) const;
		/// @copydoc Codec::codeToFile
		void codeToFile( DataStreamInterface* _input, const String& _outFileName, CodecData* _data ) const;
		/// @copydoc Codec::decode
		DecodeResult decode( DataStreamInterface* _input ) const;

		String getType() const;        

		/// Static method to startup FreeImage and register the FreeImage codecs
		static void startup();
		/// Static method to shutdown FreeImage and unregister the FreeImage codecs
		static void shutdown();

		bool getDataInfo( DataStreamInterface* _inputData, CodecData* _codecData ) const { return false; }
		bool decode( DataStreamInterface* _input, unsigned char* _buffer, unsigned int _options ) const { return false; }

	private:
		String m_type;
		unsigned int m_freeImageType;

		typedef std::list<ImageCodec*> TRegisteredCodecList;
		static TRegisteredCodecList ms_codecList;

		/** Common encoding routine. */
		FIBITMAP* encode( DataStreamInterface* _input, CodecData* _data ) const;
	};

} // namespace Menge
