#include "ScriptModuleLoader.h"

#include "Interface/ScriptCodeDataInterface.h"

#include "Kernel/Dataflow.h"

namespace Mengine
{
    ScriptModuleLoader::ScriptModuleLoader()
        : m_kernel( nullptr )
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
    void ScriptModuleLoader::setDataflow( const DataflowInterfacePtr & _dataflow )
    {
        m_dataflow = _dataflow;
    }
    //////////////////////////////////////////////////////////////////////////
    const DataflowInterfacePtr & ScriptModuleLoader::getDataflow() const
    {
        return m_dataflow;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ScriptModuleLoader::initialize( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath )
    {
        m_fileGroup = _fileGroup;
        m_filePath = _filePath;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ScriptModuleLoader::setPackagePath( bool _packagePath )
    {
        m_packagePath = _packagePath;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ScriptModuleLoader::load_module( pybind::kernel_interface * _kernel, PyObject * _moduleName )
    {
        ScriptCodeDataInterfacePtr codeData = Helper::popDataflow( m_fileGroup, m_filePath, m_dataflow );

        if( codeData == nullptr )
        {
            return nullptr;
        }

        const pybind::object & code = codeData->getScriptCode();

        if( code == nullptr )
        {
            return nullptr;
        }

        const Char * str_moduleName = _kernel->string_to_char( _moduleName );

        PyObject * py_module = m_kernel->module_init( str_moduleName );

        PyObject * dict = m_kernel->module_dict( py_module );

        if( m_packagePath == true )
        {
            pybind::list py_packagePath = pybind::make_list_t( m_kernel, _moduleName );
            pybind::dict_setstring_i( m_kernel, dict, "__path__", py_packagePath );
        }

        PyObject * py_module_exec = m_kernel->module_execcode( str_moduleName, code.ptr() );

        return py_module_exec;
    }
}