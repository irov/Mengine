#	pragma once

#include "ImageCodec.h"
// Forward-declaration to avoid external dependency on FreeImage
struct FIBITMAP;

#	include <list>

namespace Menge
{	
	class MengeCodec : public ImageCodec
    {
    private:
        String mType;

		/// Single registered codec instance
		static MengeCodec* msInstance;
	public:
        MengeCodec();
        virtual ~MengeCodec();

        DataStreamInterface * code(DataStreamInterface * input, CodecData * pData) const;
        /// @copydoc Codec::codeToFile
        void codeToFile(DataStreamInterface * input, const String& outFileName, CodecData * pData) const;
        /// @copydoc Codec::decode
        DecodeResult decode(DataStreamInterface * input) const;
        
        virtual String getType() const;        

		/// Static method to startup and register the DDS codec
		static void startup(void);
		/// Static method to shutdown and unregister the DDS codec
		static void shutdown(void);

		bool getDataInfo( DataStreamInterface* _inputData, CodecData* _codecData ) const { return false; }
		bool decode( DataStreamInterface* _input, unsigned char* _buffer, unsigned int _options ) const { return false; }

    };
}