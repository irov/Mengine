#	include "RegisterService.h"

#	include "Logger/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( Menge::RegisterInterface, Menge::RegisterService );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RegisterService::RegisterService()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	RegisterService::~RegisterService()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool RegisterService::_initialize()
	{
		for( uint32_t i = 0; i != MENGINE_REGISTER_MAX_COUNT; ++i )
		{
			RegisterValue & value = m_values;

			value.category = nullptr;
			value.key = nullptr;
			value.type = ERT_UNKNOWN;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void RegisterService::_finalize()
	{			
		for( uint32_t i = 0; i != MENGINE_REGISTER_MAX_COUNT; ++i )
		{
			RegisterValue & value = m_values;

			value.value_const_string.clear();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool RegisterService::addRegisterValue( const char * _key, const char * _value )
	{
		for( uint32_t i = 0; i != MENGINE_REGISTER_MAX_COUNT; ++i )
		{
			RegisterValue & value = m_values;
			
			if( value.key == nullptr )
			{
				return false;
			}

			value.key = _key;
			value.type = ERT_STRING;
			strcpy( value.value_str, _value );

			return true;
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool RegisterService::addRegisterValue( const char * _key, int _value )
	{
		for( uint32_t i = 0; i != MENGINE_REGISTER_MAX_COUNT; ++i )
		{
			RegisterValue & value = m_values;

			if( value.key == nullptr )
			{
				return false;
			}

			value.key = _key;
			value.type = ERT_INT;
			value.value_int = _value;

			return true;
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool RegisterService::addRegisterValue( const char * _key, uint32_t _value )
	{

	}
	//////////////////////////////////////////////////////////////////////////
	bool RegisterService::addRegisterValue( const char * _key, float _value )
	{

	}
	//////////////////////////////////////////////////////////////////////////
	bool RegisterService::addRegisterValue( const char * _key, const ConstString & _value )
	{

	}
	//////////////////////////////////////////////////////////////////////////
	const char * RegisterService::getRegisterValue( const char * _key, const char * _default )
	{

	}
	//////////////////////////////////////////////////////////////////////////
	int RegisterService::getRegisterValue( const char * _key, int _default )
	{

	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t RegisterService::getRegisterValue( const char * _key, uint32_t _default )
	{

	}
	//////////////////////////////////////////////////////////////////////////
	float RegisterService::getRegisterValue( const char * _key, float _default )
	{

	}
	//////////////////////////////////////////////////////////////////////////
	ConstString RegisterService::getRegisterValue( const char * _key, const ConstString & _default )
	{

	}
}