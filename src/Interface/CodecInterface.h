#	pragma once

#	include "Config/Typedef.h"

#	include "FileSystemInterface.h"

namespace Menge
{
	class CodecInterface
		//: public DataStreamInterface
	{
	public:
		class CodecData 
		{
		public:
			virtual ~CodecData() {};
		};

	public:
		//virtual ~CodecInterface() {};

		virtual bool code( OutStreamInterface* _outStream, unsigned char* _buffer, CodecData* _data ) const = 0;
		virtual bool getDataInfo( DataStreamInterface* _inputData, CodecData* _codecData ) const = 0;
		virtual bool decode( DataStreamInterface* _input, unsigned char* _buffer, unsigned int _options ) const = 0;

		virtual bool start( DataStreamInterface* _inputData, CodecData* _codecData ) = 0;
		virtual int sync( float _timing ) = 0;
		virtual void finish() = 0;
		virtual std::streamsize read( void* _buf, std::streamsize _count ) = 0;
		virtual bool eof() const = 0;


	};

}	// namespace Menge