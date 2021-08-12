#include "PythonScriptModuleLoader.h"

#include "Interface/ScriptCodeDataInterface.h"

#include "Kernel/Dataflow.h"
#include "Kernel/Document.h"

namespace Mengine
{
    PythonScriptModuleLoader::PythonScriptModuleLoader()
        : m_kernel( nullptr )
        , m_module( nullptr )
        , m_packagePath( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PythonScriptModuleLoader::~PythonScriptModuleLoader()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonScriptModuleLoader::setModule( pybind::kernel_interface * _kernel, PyObject * _module )
    {
        m_kernel = _kernel;
        m_module = _module;
        m_kernel->incref( m_module );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * PythonScriptModuleLoader::getModule() const
    {
        return m_module;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonScriptModuleLoader::setDataflow( const DataflowInterfacePtr & _dataflow )
    {
        m_dataflow = _dataflow;
    }
    //////////////////////////////////////////////////////////////////////////
    const DataflowInterfacePtr & PythonScriptModuleLoader::getDataflow() const
    {
        return m_dataflow;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonScriptModuleLoader::initialize( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath )
    {
        m_fileGroup = _fileGroup;
        m_filePath = _filePath;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonScriptModuleLoader::finalize()
    {
        m_dataflow = nullptr;
        m_fileGroup = nullptr;

        if( m_kernel != nullptr )
        {
            m_kernel->decref( m_module );
            m_module = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonScriptModuleLoader::setPackagePath( bool _packagePath )
    {
        m_packagePath = _packagePath;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * PythonScriptModuleLoader::load_module( pybind::kernel_interface * _kernel, PyObject * _moduleName )
    {
        DataflowContext context;
        context.filePath = m_filePath;

        ScriptCodeDataInterfacePtr codeData = Helper::popDataflow( m_fileGroup, m_filePath, m_dataflow, &context, MENGINE_DOCUMENT_FACTORABLE );

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
    //////////////////////////////////////////////////////////////////////////
}