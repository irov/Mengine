#pragma once

#include "Config/Typedef.h"
#include "Config/Char.h"

namespace Mengine
{
	class SecureValue
	{
	public:
		SecureValue();
		~SecureValue();

	public:
		void setUnprotectedValue( uint32_t _value );
		bool getUnprotectedValue( uint32_t * _value ) const;

		bool addSecureValue( const SecureValue & _value );

	public:
		uint32_t m_value;
		uint32_t m_hash;
		Char m_buffer[17];
	};
}