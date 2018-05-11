#pragma once

#include "Interface/StreamInterface.h"
#include "Interface/FileSystemInterface.h"
#include "Interface/ArchiveInterface.h"

#include "ScriptModuleLoaderSource.h"
#include "ScriptModuleLoaderCode.h"

#include "Factory/Factory.h"

#include "Kernel/Servant.h"
#include "Core/ConstString.h"
#include "Core/ConstStringTypes.h"
#include "Core/FilePath.h"

#include "pybind/pybind.hpp"

namespace Mengine
{    
	//////////////////////////////////////////////////////////////////////////
	struct ModulePathes
	{
		ConstString pack;
		TVectorConstString pathes;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::vector<ModulePathes> TVectorModulePathes;
	//////////////////////////////////////////////////////////////////////////
    class ScriptModuleFinder
		: public Servant
    {
    public:
        ScriptModuleFinder();

	public:
		bool initialize();
		void finalize();

	public:
		void setEmbed( PyObject * _embed );

    public:
        void addModulePath( const ConstString & _pack, const TVectorConstString & _pathes );
		void removeModulePath( const ConstString & _pack );

    public:
		PyObject * find_module( PyObject * _module, PyObject * _path );
		PyObject * load_module( PyObject * _module );

    protected:
        bool find_module_source_( PyObject * _module, const ScriptModuleLoaderPtr & _loader );
        bool find_module_code_( PyObject * _module, const ScriptModuleLoaderPtr & _loader );        
		bool find_module_( PyObject * _module, const ScriptModuleLoaderPtr & _loader, const char * _ext, size_t _extN, const char * _init, size_t _extI );

    protected:
        bool convertDotToSlash_( char * _cache, size_t _cacheSize, PyObject * _module, size_t & _modulePathCacheLen );

	protected:
		bool findModule_( const char * _modulePath, size_t _modulePathLen, const ScriptModuleLoaderPtr & _loader ) const;

    protected:
		PyObject * m_embed;

		ArchivatorInterfacePtr m_archivator;

        TVectorModulePathes m_modulePaths;    

		FactoryPtr m_factoryScriptModuleLoaderCode;

#ifndef MENGINE_MASTER_RELEASE
		FactoryPtr m_factoryScriptModuleLoaderSource;
#endif

		typedef stdex::vector<ScriptModuleLoaderPtr> TMapModuleLoaders;
		TMapModuleLoaders m_loaders;

		mutable PathString m_cacheFullPath;
    };
	//////////////////////////////////////////////////////////////////////////
	typedef IntrusivePtr<ScriptModuleFinder> ScriptModuleFinderPtr;
	//////////////////////////////////////////////////////////////////////////
}