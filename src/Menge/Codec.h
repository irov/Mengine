#	pragma once

#	include <map>

#	include "Config/Typedef.h"
#	include "Interface/FileSystemInterface.h"

namespace Menge 
{
	class Codec
	{
	public:
		Codec();
		virtual ~Codec();

	public:
		class CodecData 
		{
		public:
			virtual ~CodecData() {};
		};

	public:
		virtual bool code( OutStreamInterface* _outStream, unsigned char* _buffer, CodecData* _data ) const = 0;
		virtual bool getDataInfo( DataStreamInterface* _inputData, CodecData* _codecData ) const = 0;
		virtual bool decode( DataStreamInterface* _input, unsigned char* _buffer, unsigned int _options ) const = 0;
		virtual const String & getType() const = 0;
	};
}