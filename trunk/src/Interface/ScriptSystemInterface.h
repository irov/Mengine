#	pragma once

#	include "ServiceInterface.h"

#	include "Config/Typedef.h"

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
    class Node;
    class Entity;

    class ScriptClassInterface
    {
    public:
        virtual PyObject * wrap( Node * _node ) = 0;

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
        virtual PyObject * wrap( Node * _node ) = 0;

		virtual void addModulePath( const ConstString & _pak, const TVectorFilePath & _pathes ) = 0;

		virtual PyObject * importModule( const ConstString& _name ) = 0;

		virtual void setCurrentModule( PyObject * _module ) = 0;
        virtual void addGlobalModule( const String & _name, PyObject * _module ) = 0;

		virtual PyObject * getModuleFunction( PyObject * _module, const char * _name ) = 0;

        virtual void callFunctionVA( PyObject * _object, const char * _params, va_list ) = 0;
        virtual PyObject * askFunctionVA( PyObject * _object, const char * _params, va_list ) = 0;

		virtual void callFunction( PyObject * _object, const char * _params, ... ) = 0;
        virtual PyObject * askFunction( PyObject * _object, const char * _params, ... ) = 0;

        virtual Entity * createEntity( const ConstString& _type, const ConstString & _prototype, PyObject * _generator ) = 0;

        template<class T>
        T * createEntityT( const ConstString& _type, const ConstString & _prototype, PyObject * _generator )
        {
            Entity * entity = this->createEntity( _type, _prototype, _generator );

            if( dynamic_cast<T*>(entity) == NULL )
            {
                return NULL;
            }

            T * t = static_cast<T*>(entity);

            return t;
        }

    public:
        virtual ConstString stringize( PyObject * _str, bool & _valid ) = 0;
	};

#   define SCRIPT_SERVICE( serviceProvider )\
    (Menge::getService<Menge::ScriptServiceInterface>(serviceProvider))
}
