#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/FileGroupInterface.h"
#include "Interface/ContentInterface.h"
#include "Interface/PrefetcherObserverInterface.h"
#include "Interface/ScriptModuleInterface.h"
#include "Interface/ScriptEmbeddingInterface.h"

#include "Environment/Python/PythonForwardDeclaration.h"

#include "Kernel/ConstString.h"
#include "Kernel/Eventable.h"
#include "Kernel/FilePath.h"
#include "Kernel/Tags.h"
#include "Kernel/Vector.h"

//////////////////////////////////////////////////////////////////////////
namespace pybind
{
    class object;
}
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct ScriptModulePackage
    {
        ContentInterfacePtr content;
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

        virtual void prefetchModules( const PrefetcherObserverInterfacePtr & _cb, const DocumentInterfacePtr & _doc ) = 0;

    public:
        virtual void addModulePath( const FileGroupInterfacePtr & _fileGroup, const VectorScriptModulePack & _modules ) = 0;
        virtual void removeModulePath( const FileGroupInterfacePtr & _fileGroup, const VectorScriptModulePack & _modules ) = 0;

    public:
        virtual ScriptModuleInterfacePtr importModule( const ConstString & _name ) = 0;

    public:
        virtual void addGlobalModule( const Char * _name, PyObject * _module ) = 0;
        virtual PyObject * getGlobalModule( const Char * _name ) const = 0;
        virtual void removeGlobalModule( const Char * _name ) = 0;

    public:
        virtual void setAvailablePlugin( const Char * _name, bool _available ) = 0;
        virtual bool isAvailablePlugin( const Char * _name ) const = 0;

    public:
        virtual bool addScriptEmbedding( const ConstString & _name, const ScriptEmbeddingInterfacePtr & _embedding ) = 0;
        virtual void removeScriptEmbedding( const ConstString & _name ) = 0;

    public:
        virtual EventablePtr eventableEntity( const pybind::object & _type ) = 0;

    public:
        virtual void stringize( PyObject * _object, ConstString * const _str ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define SCRIPT_SERVICE()\
    ((Mengine::ScriptServiceInterface*)SERVICE_GET(Mengine::ScriptServiceInterface))
//////////////////////////////////////////////////////////////////////////