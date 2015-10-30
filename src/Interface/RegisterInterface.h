#	pragma once

#	include "Interface/ServiceInterface.h"

#	include "Core/ConstString.h"

namespace Menge
{	
	class RegisterInterface
		: public ServiceInterface
	{
        SERVICE_DECLARE("Register")

	public:
		virtual bool addRegisterValue( const char * _category, const char * _key, const char * _value );
		virtual bool addRegisterValue( const char * _category, const char * _key, int _value );
		virtual bool addRegisterValue( const char * _category, const char * _key, uint32_t _value );
		virtual bool addRegisterValue( const char * _category, const char * _key, float _value );
		virtual bool addRegisterValue( const char * _category, const char * _key, const ConstString & _value );

	public:
		virtual const char * getRegisterValue( const char * _category, const char * _key, const char * _default );
		virtual int getRegisterValue( const char * _category, const char * _key, int _default );
		virtual uint32_t getRegisterValue( const char * _category, const char * _key, uint32_t _default );
		virtual float getRegisterValue( const char * _category, const char * _key, float _default );
		virtual ConstString getRegisterValue( const char * _category, const char * _key, const ConstString & _default );
	};

#   define REGISTER_SERVICE(serviceProvider)\
    ((Menge::RegisterInterface*)SERVICE_GET(serviceProvider, Menge::RegisterInterface))
}

