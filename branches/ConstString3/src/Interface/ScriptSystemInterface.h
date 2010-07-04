#	pragma once

#	include "Config/Typedef.h"

#	include <list>

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
		virtual void onCall( ScriptObjectInterface * _script ) = 0;
	};

	class ScriptTypeInterface
	{
	public:
		virtual void addMethod( const char * _name, ScriptMethodInterface * _method ) = 0;
	};

	class ScriptSystemInterface
	{
	public:
		virtual bool initialize() = 0;
		virtual void finialize() = 0;

	public:
		typedef std::list<String> TListModulePath;
		virtual void addModulePath( const TListModulePath& _listPath ) = 0;

	public:
		virtual ScriptTypeInterface * registerClass( const char * _name, const std::type_info & _info ) = 0;
		virtual ScriptTypeInterface * importClass( const char * _name, const char * _category ) = 0;

		virtual ScriptObjectInterface * createObject( ScriptTypeInterface * _type );
	};
}