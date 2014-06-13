#	pragma once

#	include "Interface/ServiceInterface.h"
#	include "Interface/PrototypeManagerInterface.h"

#	include "Config/Typedef.h"
#	include "Config/String.h"

#	include "Core/ConstString.h"
#	include "Core/FilePath.h"

#	include <vector>

#	include <stdarg.h>

extern "C" 
{ 
	struct _object;
	typedef _object PyObject;
}

namespace Menge
{	    
    class Scriptable;
	class Eventable;

    class Entity;

    class ScriptClassInterface
    {
    public:
        virtual PyObject * wrap( Scriptable * _node ) = 0;

    public:
        virtual void destroy() = 0;
    };

	class ScriptServiceInterface
		: public ServiceInterface
	{
        SERVICE_DECLARE("ScriptService")

    public:
        virtual bool initialize() = 0;
        virtual void finalize() = 0;

	public:
        virtual void addWrapping( const ConstString& _type, ScriptClassInterface * _wrapper ) = 0;
        virtual PyObject * wrap( const ConstString & _type, Scriptable * _node ) = 0;

	public:
		virtual bool bootstrapModules() = 0;

	public:
		virtual void addModulePath( const ConstString & _pak, const TVectorFilePath & _pathes, const TVectorConstString & _modules ) = 0;

		virtual PyObject * importModule( const ConstString& _name ) = 0;

		virtual void setCurrentModule( PyObject * _module ) = 0;
        virtual void addGlobalModule( const String & _name, PyObject * _module ) = 0;
        virtual void removeGlobalModule( const String & _name ) = 0;

		virtual PyObject * getModuleFunction( PyObject * _module, const char * _name ) = 0;

        virtual void callFunctionVA( PyObject * _object, const char * _params, va_list ) = 0;
        virtual PyObject * askFunctionVA( PyObject * _object, const char * _params, va_list ) = 0;

		virtual void callFunction( PyObject * _object, const char * _params, ... ) = 0;
        virtual PyObject * askFunction( PyObject * _object, const char * _params, ... ) = 0;

	public:
		virtual PrototypeGeneratorInterfacePtr createEntityGenerator( const ConstString & _category, const ConstString & _prototype, PyObject * _generator ) = 0;
		virtual PyObject * importEntity( const ConstString & _category, const ConstString & _prototype ) = 0;

	public:
        virtual Entity * createEntity( const ConstString& _type, const ConstString & _prototype, PyObject * _generator, Eventable * _eventable ) = 0;

        template<class T>
        T * createEntityT( const ConstString& _type, const ConstString & _prototype, PyObject * _generator, Eventable * _eventable )
        {
            Entity * entity = this->createEntity( _type, _prototype, _generator, _eventable );

#   ifdef _DEBUG
            if( dynamic_cast<T*>(entity) == nullptr )
            {
                return nullptr;
            }
#   endif

            T * t = static_cast<T*>(entity);

            return t;
        }

    public:
        virtual bool stringize( PyObject * _object, ConstString & _str ) = 0;
	};

#   define SCRIPT_SERVICE( serviceProvider )\
    ((Menge::ScriptServiceInterface*)SERVICE_GET(serviceProvider, Menge::ScriptServiceInterface))
}
