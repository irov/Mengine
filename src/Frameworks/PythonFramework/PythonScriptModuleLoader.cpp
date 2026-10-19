#include "PythonScriptModuleLoader.h"

#include "Interface/ScriptCodeDataInterface.h"

#include "Kernel/Dataflow.h"

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
    bool PythonScriptModuleLoader::initialize( const ContentInterfacePtr & _content )
    {
        m_content = _content;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonScriptModuleLoader::finalize()
    {
        m_dataflow = nullptr;
        m_content = nullptr;

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
        const FilePath & filePath = m_content->getFilePath();

        // The name is taken first: a load that fails raises in the import that
        // asked for it, which a null result without an error would turn into a
        // SystemError.
        const Char * str_moduleName = _kernel->string_to_char( _moduleName );

        DataflowContext context;
        context.filePath = filePath;

        ScriptCodeDataInterfacePtr codeData = Helper::popDataflow( m_content, m_dataflow, &context, MENGINE_DOCUMENT_FACTORABLE );

        if( codeData == nullptr )
        {
            m_kernel->error_message( "invalid load module '%s' (dataflow)"
                , str_moduleName
            );

            return nullptr;
        }

        const pybind::object & code = codeData->getScriptCode();

        if( code == nullptr )
        {
            m_kernel->error_message( "invalid load module '%s' (code)"
                , str_moduleName
            );

            return nullptr;
        }

        PyObject * py_module = m_kernel->module_init( str_moduleName );

        PyObject * dict = m_kernel->module_dict( py_module );

        if( m_packagePath == true )
        {
            pybind::list py_packagePath = pybind::make_list_t( m_kernel, _moduleName );
            pybind::dict_setstring_i( m_kernel, dict, "__path__", py_packagePath );
        }

#if defined(MENGINE_DEBUG)
        pybind::dict_setstring_t( m_kernel, dict, "__content__", m_content );
#endif

        PyObject * py_module_exec = m_kernel->module_execcode( str_moduleName, code.ptr() );

        // The code reported its error as it ran.
        if( py_module_exec == nullptr )
        {
            m_kernel->error_message( "invalid load module '%s' (script)"
                , str_moduleName
            );

            return nullptr;
        }

        return py_module_exec;
    }
    //////////////////////////////////////////////////////////////////////////
}