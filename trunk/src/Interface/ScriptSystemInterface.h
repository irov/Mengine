#	pragma once

#	include "Interface/ServiceInterface.h"
#	include "Interface/PrototypeManagerInterface.h"

#	include "Config/Typedef.h"
#	include "Config/String.h"

#	include "Core/ConstString.h"
#	include "Core/FilePath.h"

#	include "pybind/object.hpp"

#	include <stdex/stl_vector.h>

#	include <stdarg.h>

namespace Menge
{	    
    class Scriptable;
	class Eventable;

    class Entity;

    class ScriptClassInterface
    {
	public:
		ScriptClassInterface(){};
		virtual ~ScriptClassInterface(){};

    public:
		virtual pybind::object wrap( Scriptable * _node ) = 0;

    public:
        virtual void destroy() = 0;
    };

	struct ScriptModulePak
	{
		ConstString path;
		ConstString module;
		ConstString initializer;
	};

	typedef stdex::vector<ScriptModulePak> TVectorScriptModulePak;

	class ScriptServiceInterface
		: public ServiceInterface
	{
        SERVICE_DECLARE("ScriptService")

    public:
        virtual bool initialize() = 0;
        virtual void finalize() = 0;

	public:
        virtual void addWrapping( const ConstString& _type, ScriptClassInterface * _wrapper ) = 0;
		virtual pybind::object wrap( const ConstString & _type, Scriptable * _node ) = 0;

	public:
		virtual bool bootstrapModules() = 0;
		virtual bool initializeModules() = 0;

	public:
		virtual void addModulePath( const ConstString & _pak, const TVectorScriptModulePak & _modules ) = 0;

		virtual pybind::object importModule( const ConstString& _name ) = 0;

		virtual void setCurrentModule( PyObject * _module ) = 0;
        virtual void addGlobalModule( const String & _name, PyObject * _module ) = 0;
        virtual void removeGlobalModule( const String & _name ) = 0;

		virtual bool hasModuleFunction( const pybind::object & _module, const char * _name ) = 0;
		virtual pybind::object getModuleFunction( const pybind::object & _module, const char * _name ) = 0;

	public:
		virtual PrototypeGeneratorInterfacePtr createEntityGenerator( const ConstString & _category, const ConstString & _prototype, const pybind::object & _generator ) = 0;
		virtual pybind::object importEntity( const ConstString & _category, const ConstString & _prototype ) = 0;

	public:
		virtual Entity * createEntity( const ConstString& _type, const ConstString & _prototype, const pybind::object & _generator, Eventable * _eventable ) = 0;

        template<class T>
		T * createEntityT( const ConstString& _type, const ConstString & _prototype, const pybind::object & _generator, Eventable * _eventable )
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
