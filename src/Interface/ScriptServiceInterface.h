#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/ScriptModuleInterface.h"
#include "Interface/ScriptCodeDataInterface.h"
#include "Interface/ScriptEmbeddingInterface.h"
#include "Interface/FileGroupInterface.h"
#include "Interface/PrefetcherObserverInterface.h"

#include "Config/Typedef.h"

#include "Kernel/ScriptWrapperInterface.h"
#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"
#include "Kernel/Tags.h"

#include "Config/Vector.h"

#include "pybind/kernel.hpp"
#include "pybind/base.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct ScriptModulePack
    {
        FileGroupInterfacePtr fileGroup;
        FilePath path;
        ConstString module;
        ConstString initializer;
        ConstString finalizer;
        Tags platform;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<ScriptModulePack> VectorScriptModulePack;
    //////////////////////////////////////////////////////////////////////////
    class ScriptServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "ScriptService" )

    public:
        virtual pybind::kernel_interface * getKernel() = 0;

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
        virtual void ejectingScriptEmbeddings() = 0;

    public:
        template<class T>
        void addGlobalModuleT( const Char * _name, const T & _value )
        {
            pybind::kernel_interface * kernel = this->getKernel();

            PyObject * py_value = pybind::ptr( kernel, _value );
            this->addGlobalModule( _name, py_value );
        }

    public:
        virtual bool stringize( PyObject * _object, ConstString & _str ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define SCRIPT_SERVICE()\
    ((Mengine::ScriptServiceInterface*)SERVICE_GET(Mengine::ScriptServiceInterface))
//////////////////////////////////////////////////////////////////////////
#define ADD_SCRIPT_EMBEDDING(NAME)\
    if(SCRIPT_SERVICE()->addScriptEmbedding( STRINGIZE_STRING_LOCAL(#NAME), Helper::makeFactorableUnique<NAME>())==false) return false
//////////////////////////////////////////////////////////////////////////
#define REMOVE_SCRIPT_EMBEDDING(NAME)\
    SCRIPT_SERVICE()->removeScriptEmbedding( STRINGIZE_STRING_LOCAL(#NAME))
//////////////////////////////////////////////////////////////////////////