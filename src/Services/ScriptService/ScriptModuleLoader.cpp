#include "ScriptModuleLoader.h"

namespace Mengine
{
    ScriptModuleLoader::ScriptModuleLoader()
        : m_kernel(nullptr)
        , m_module( nullptr )
        , m_packagePath( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ScriptModuleLoader::~ScriptModuleLoader()
    {
        m_kernel->decref( m_module );
    }
    //////////////////////////////////////////////////////////////////////////
    void ScriptModuleLoader::setModule( pybind::kernel_interface * _kernel, PyObject * _module )
    {
        m_kernel = _kernel;
        m_module = _module;
        m_kernel->incref( m_module );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ScriptModuleLoader::getModule() const
    {
        return m_module;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ScriptModuleLoader::initialize( const FileGroupInterfacePtr & _group, const FilePath & _path, const ArchivatorInterfacePtr & _archivator )
    {
        m_group = _group;
        m_path = _path;
        m_archivator = _archivator;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ScriptModuleLoader::setPackagePath( bool _packagePath )
    {
        m_packagePath = _packagePath;
    }
}