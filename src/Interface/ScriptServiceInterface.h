#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/FileGroupInterface.h"
#include "Interface/PrefetcherObserverInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/Eventable.h"
#include "Kernel/FilePath.h"
#include "Kernel/Tags.h"

#include "Config/Typedef.h"
#include "Config/Vector.h"

extern "C"
{
    struct _object;
    typedef _object PyObject;
}

namespace pybind
{
    class kernel_interface;
    class object;
}

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class ScriptModuleInterface> ScriptModuleInterfacePtr;
    typedef IntrusivePtr<class ScriptCodeDataInterface, class DataInterface> ScriptCodeDataInterfacePtr;
    typedef IntrusivePtr<class ScriptEmbeddingInterface> ScriptEmbeddingInterfacePtr;
    typedef IntrusivePtr<class ScriptWrapperInterface> ScriptWrapperInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    struct ScriptModulePackage
    {
        FileGroupInterfacePtr fileGroup;
        FilePath path;
        ConstString module;
        ConstString initializer;
        ConstString finalizer;
        Tags platform;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<ScriptModulePackage> VectorScriptModulePack;
    //////////////////////////////////////////////////////////////////////////
    class ScriptServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "ScriptService" )

    public:
        virtual bool bootstrapModules() = 0;
        virtual bool initializeModules() = 0;
        virtual bool finalizeModules() = 0;

        virtual void prefetchModules( const PrefetcherObserverInterfacePtr & _cb ) = 0;

    public:
        virtual void addModulePath( const FileGroupInterfacePtr & _fileGroup, const VectorScriptModulePack & _modules ) = 0;
        virtual void removeModulePath( const FileGroupInterfacePtr & _fileGroup, const VectorScriptModulePack & _modules ) = 0;

        virtual ScriptModuleInterfacePtr importModule( const ConstString & _name ) = 0;

        virtual void setCurrentModule( PyObject * _module ) = 0;
        virtual void addGlobalModule( const Char * _name, PyObject * _module ) = 0;
        virtual void removeGlobalModule( const Char * _name ) = 0;

    public:
        virtual bool addScriptEmbedding( const ConstString & _name, const ScriptEmbeddingInterfacePtr & _embedding ) = 0;
        virtual void removeScriptEmbedding( const ConstString & _name ) = 0;

    public:
        virtual EventablePtr eventableEntity( const pybind::object & _type ) = 0;

    public:
        virtual bool stringize( PyObject * _object, ConstString & _str ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define SCRIPT_SERVICE()\
    ((Mengine::ScriptServiceInterface*)SERVICE_GET(Mengine::ScriptServiceInterface))
//////////////////////////////////////////////////////////////////////////
#define ADD_SCRIPT_EMBEDDING(Name, Embedding)\
    SCRIPT_SERVICE()->addScriptEmbedding( Name, Embedding )
//////////////////////////////////////////////////////////////////////////
#define REMOVE_SCRIPT_EMBEDDING(Name)\
    SCRIPT_SERVICE()->removeScriptEmbedding( Name )
//////////////////////////////////////////////////////////////////////////