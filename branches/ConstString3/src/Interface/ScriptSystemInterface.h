#	pragma once

namespace Menge
{
	class ScriptObjectInterface
	{
	public:
		virtual ScriptObjectInterface * call( const char * _method, const char * _param, ... ) = 0;

		virtual bool setattr( const char * _attr, ScriptObjectInterface * _value ) = 0;
		virtual ScriptObjectInterface * getattr( const char * _attr ) = 0;
	};

	class ScriptSystem
	{
	public:
		virtual bool initialize() = 0;

	public:
		virtual void registerClass( const char * _name, const std::type_info & _info,   )
	};
}