#	pragma once

namespace Menge
{
	class ScriptObjectInterface
	{
	public:
		virtual ScriptObjectInterface * callMethod( const char * _method, const char * _param, ... ) = 0;

		virtual bool setAttribute( const char * _attr, ScriptObjectInterface * _value ) = 0;
		virtual ScriptObjectInterface * getAttribute( const char * _attr ) = 0;
	};

	class ScriptMethodInterface
	{
	public:
		virtual void onCall( ScriptObjectInterface * _scriptable )
	};

	class ScriptTypeInterface
	{
	public:
		virtual void addMethod( const char * _name, ScriptMethodInterface * _method )
	};

	class ScriptSystem
	{
	public:
		virtual bool initialize() = 0;

	public:
		virtual ScriptTypeInterface * registerClass( const char * _name, const std::type_info & _info,   )
	};
}