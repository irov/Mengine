#   pragma once

#   include "Interface/RegisterInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
#	ifndef MENGINE_REGISTER_MAX_COUNT
#	define MENGINE_REGISTER_MAX_COUNT
#	endif
	//////////////////////////////////////////////////////////////////////////
    class RegisterService
		: public ServiceBase<RegisterInterface>
    {
    public:
		RegisterService();
		~RegisterService();

	public:
		bool _initialize() override;
		void _finalize() override;

	public:
		bool addRegisterValue( const char * _key, const char * _value ) override;
		bool addRegisterValue( const char * _key, int _value ) override;
		bool addRegisterValue( const char * _key, uint32_t _value ) override;
		bool addRegisterValue( const char * _key, float _value ) override;
		bool addRegisterValue( const char * _key, const ConstString & _value ) override;

	public:
		const char * getRegisterValue( const char * _key, const char * _default ) override;
		int getRegisterValue( const char * _key, int _default ) override;
		uint32_t getRegisterValue( const char * _key, uint32_t _default ) override;
		float getRegisterValue( const char * _key, float _default ) override;
		ConstString getRegisterValue( const char * _key, const ConstString & _default ) override;

	protected:
		enum ERegisterType
		{
			ERT_STRING,
			ERT_INT,
			ERT_UINT,
			ERT_FLOAT,
			ERT_CONST_STRING,
			ERT_UNKNOWN
		};

		struct RegisterValue
		{
			const char * key;
			ERegisterType type;

			char value_str[256];
			int value_int;
			uint32_t value_uint;
			float value_float;
			ConstString value_const_string;
		};

		RegisterValue m_values[MENGINE_REGISTER_MAX_COUNT];
    };
}