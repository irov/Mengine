#	pragma once

#	include "Config/Typedef.h"

#	include "Core/Holder.h"
#	include "Factory/FactoryManager.h"

namespace Menge 
{
	class EncoderInterface;

	class EncoderManager
		: public FactoryManager
		, public Holder<EncoderManager>
	{
	public:
		EncoderInterface * createEncoder( const String& _fileSystemName, const String& _filename, const String& _type );
		EncoderInterface * createEncoder( const String& _filename, const String & _type, FileOutputInterface * _stream ) override;

		template<class T>
		T* createEncoderT( const String& _fileSystemName, const String& _filename, const String& _type )
		{
			return static_cast<T*>( createEncoder( _fileSystemName, _filename, _type ) );
		}

		void releaseEncoder( EncoderInterface * _encoder );
	};
} // namespace Menge
