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
	//////////////////////////////////////////////////////////////////////////
    class ScriptWrapperInterface
    {
	public:
		ScriptWrapperInterface(){};
		virtual ~ScriptWrapperInterface(){};

	public:
		virtual void setServiceProvider( ServiceProviderInterface * _serviceProvider ) = 0;
		virtual ServiceProviderInterface * getServiceProvider() = 0;

	public:
		virtual bool initialize() = 0;
		virtual void finalize() = 0;

    public:
		virtual PyObject * wrap( Scriptable * _node ) = 0;

    public:
        virtual void destroy() = 0;
    };
	//////////////////////////////////////////////////////////////////////////
	struct ScriptModulePack
	{
		ConstString path;
		ConstString module;
		ConstString initializer;
		ConstString finalizer;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::vector<ScriptModulePack> TVectorScriptModulePack;
	//////////////////////////////////////////////////////////////////////////
	class ScriptServiceInterface
		: public ServiceInterface
	{
        SERVICE_DECLARE("ScriptService")

	public:
		virtual void setWrapper( const ConstString& _type, ScriptWrapperInterface * _wrapper ) = 0;
		virtual ScriptWrapperInterface * getWrapper( const ConstString & _type ) const = 0;

	public:
		virtual bool bootstrapModules() = 0;
		virtual bool initializeModules() = 0;
		virtual bool finalizeModules() = 0;

	public:
		virtual void addModulePath( const ConstString & _pack, const TVectorScriptModulePack & _modules ) = 0;
		virtual void removeModulePath( const ConstString & _pack, const TVectorScriptModulePack & _modules ) = 0;

		virtual pybind::object importModule( const ConstString& _name ) = 0;

		virtual void setCurrentModule( PyObject * _module ) = 0;
		virtual void addGlobalModule( const Char * _name, PyObject * _module ) = 0;
		virtual void removeGlobalModule( const Char * _name ) = 0;

	public:
		virtual bool addEntityPrototype( const ConstString & _category, const ConstString & _prototype, const pybind::object & _generator ) = 0;
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
