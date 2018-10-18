#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/MemoryInterface.h"

#include "Config/Typedef.h"
#include "Config/String.h"

#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"
#include "Kernel/Tags.h"

#include "Kernel/Servant.h"

#include "pybind/kernel.hpp"
#include "pybind/base.hpp"

#include "Config/Vector.h"

namespace Mengine
{
    class Scriptable;
    class Eventable;
    //////////////////////////////////////////////////////////////////////////
    class ScriptWrapperInterface
        : public ServantInterface
    {
    public:
        virtual bool initialize() = 0;
        virtual void finalize() = 0;

    public:
        virtual PyObject * wrap( Scriptable * _node ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ScriptWrapperInterface> ScriptWrapperInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    struct ScriptModulePack
    {
        FilePath path;
        ConstString module;
        ConstString initializer;
        ConstString finalizer;
        Tags platform;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<ScriptModulePack> VectorScriptModulePack;
    //////////////////////////////////////////////////////////////////////////
    class ScriptModuleInterface
        : public Servant
    {
    public:
        virtual bool onInitialize( const ConstString & _method ) = 0;
        virtual bool onFinalize( const ConstString & _method ) = 0;

    public:
        virtual const pybind::base & getModule() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ScriptModuleInterface> ScriptModuleInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class ScriptServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "ScriptService" )

    public:
        virtual pybind::kernel_interface * getKernel() = 0;

    public:
        virtual bool setWrapper( const ConstString& _type, const ScriptWrapperInterfacePtr & _wrapper ) = 0;
        virtual void removeWrapper( const ConstString& _type ) = 0;
        virtual const ScriptWrapperInterfacePtr & getWrapper( const ConstString & _type ) const = 0;

    public:
        virtual bool bootstrapModules() = 0;
        virtual bool initializeModules() = 0;
        virtual bool finalizeModules() = 0;

    public:
        virtual void addModulePath( const ConstString & _pack, const VectorScriptModulePack & _modules ) = 0;
        virtual void removeModulePath( const ConstString & _pack, const VectorScriptModulePack & _modules ) = 0;

        virtual ScriptModuleInterfacePtr importModule( const ConstString& _name ) = 0;

        virtual void setCurrentModule( PyObject * _module ) = 0;
        virtual void addGlobalModule( const Char * _name, PyObject * _module ) = 0;
        virtual void removeGlobalModule( const Char * _name ) = 0;

    public:
        virtual PyObject * loadModuleSource( PyObject * _moduleName, bool _packagePath, const MemoryInterfacePtr & _stream ) = 0;
        virtual PyObject * loadModuleBinary( PyObject * _moduleName, bool _packagePath, const MemoryInterfacePtr & _stream ) = 0;

    public:
        virtual bool stringize( PyObject * _object, ConstString & _str ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
#define SCRIPT_SERVICE()\
    ((Mengine::ScriptServiceInterface*)SERVICE_GET(Mengine::ScriptServiceInterface))
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        inline pybind::kernel_interface * getPybindkernel()
        {
            static pybind::kernel_interface * kernel = nullptr;

            if( kernel == nullptr )
            {
                kernel = SCRIPT_SERVICE()
                    ->getKernel();
            }

            return kernel;
        }
    }
}
