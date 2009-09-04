#	pragma once

#	include "Config/Typedef.h"

#	include "Holder.h"
#	include "FactoryManager.h"

namespace Menge 
{
	class Decoder;

	class DecoderManager
		: public FactoryManager
	{
	public:
		Decoder * createDecoder( const String& _fileSystemName, const String& _filename, const String& _type );

		template<class T>
		T * createDecoderT( const String& _fileSystemName, const String& _filename, const String& _type )
		{
			return static_cast<T*>( createDecoder( _fileSystemName, _filename, _type ) );
		}

		void releaseDecoder( Decoder * _decoder );
	};
} // namespace Menge
