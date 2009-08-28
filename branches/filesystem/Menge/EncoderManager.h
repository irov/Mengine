#	pragma once

#	include "Config/Typedef.h"

#	include "Holder.h"
#	include "FactoryManager.h"

namespace Menge 
{
	class Encoder;

	class EncoderManager
		: public FactoryManager
	{
	public:
		Encoder * createEncoder( const String& _filename, const String& _type );

		template<class T>
		T* createEncoderT( const String& _filename, const String& _type )
		{
			return static_cast<T*>( createEncoder( _filename, _type ) );
		}

		void releaseEncoder( Encoder* _encoder );
	};
} // namespace Menge
