#	pragma once

#	include "Config/Typedef.h"

#	include "Core/Holder.h"
#	include "FactoryManager.h"

namespace Menge 
{
	class Encoder;

	class EncoderManager
		: public FactoryManager
		, public Holder<EncoderManager>
	{
	public:
		Encoder * createEncoder( const String& _fileSystemName, const String& _filename, const String& _type );

		template<class T>
		T* createEncoderT( const String& _fileSystemName, const String& _filename, const String& _type )
		{
			return static_cast<T*>( createEncoder( _fileSystemName, _filename, _type ) );
		}

		void releaseEncoder( Encoder* _encoder );
	};
} // namespace Menge
