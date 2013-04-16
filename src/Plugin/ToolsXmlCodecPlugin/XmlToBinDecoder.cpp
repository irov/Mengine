#	include <Windows.h>
#   include <Python.h>

#	include "xml2metabuf/Xml2Metabuf.hpp"
#	include "xml2metabuf/Xml2Metacode.hpp"

#   include "Config/Typedef.h"


static PyObject * PyToolException;

//////////////////////////////////////////////////////////////////////////
static bool s_write_wstring( Metabuf::Xml2Metabuf * _metabuf, const char * _value, void * _user )
{
    size_t utf8_size = strlen( _value ); 

    _metabuf->write( utf8_size );

    _metabuf->writeCount( &_value[0], utf8_size );

    return true;
}
//////////////////////////////////////////////////////////////////////////
static bool s_write_wchar_t( Metabuf::Xml2Metabuf * _metabuf, const char * _value, void * _user )
{
    size_t utf8_size = strlen( _value ); 

    _metabuf->write( utf8_size );

    _metabuf->writeCount( &_value[0], utf8_size );

    return true;
}
//////////////////////////////////////////////////////////////////////////
//extern "C" bool writeBinary( const Menge::WChar * _protocol, const Menge::WChar * _source, const Menge::WChar * _bin, char * _error )
static PyObject * writeBin( PyObject* self, PyObject* args )
{
    const wchar_t * protocolPath;
    const wchar_t * xmlPath;
    const wchar_t * binPath;
    
    if (!PyArg_ParseTuple(args, "uuu", &protocolPath, &xmlPath, &binPath ))
    {
        char error[512];
        sprintf( error, "writeBin: error parse args"
            );

        PyErr_SetString( PyToolException, error );

        return NULL;
    }

    Metabuf::XmlProtocol xml_protocol;

    FILE * file_protocol = _wfopen( protocolPath, L"rb" );

    if( file_protocol == NULL )
    {
        char error[512];
        sprintf( error, "writeBin: error open protocol %ls"
            , protocolPath
            );

        PyErr_SetString( PyToolException, error );

        return NULL;
    }

    long size;

    fseek(file_protocol, 0, SEEK_END);
    size = ftell(file_protocol);
    fseek(file_protocol, 0, SEEK_SET);

    std::vector<char> buf(size);

    fread( &buf[0], 1, size, file_protocol );

    fclose( file_protocol );

    if( xml_protocol.readProtocol( &buf[0], size ) == false )
    {
        char error[512];
        sprintf( error, "writeBin: error read protocol %ls"
            , protocolPath
            );

        PyErr_SetString( PyToolException, error );

        return NULL;
    }

    Metabuf::Xml2Metacode xml_metacode(&xml_protocol);

    FILE * file_test = _wfopen( xmlPath, L"rb" );

    if( file_test == NULL )
    {
        char error[512];

        sprintf( error, "writeBin: error open xml %ls"
            , xmlPath
            );

        PyErr_SetString( PyToolException, error );

        return NULL;
    }

    long size_test;

    fseek(file_test, 0, SEEK_END);
    size_test = ftell(file_test);
    fseek(file_test, 0, SEEK_SET);

    Menge::TBlobject buf_test(size_test);

    fread( &buf_test[0], 1, size_test, file_test );

    fclose( file_test );

    Metabuf::Xml2Metabuf xml_metabuf(&xml_protocol);

    xml_metabuf.addSerializator( "wstring", &s_write_wstring, 0 );
    xml_metabuf.addSerializator( "wchar_t", &s_write_wchar_t, 0 );

    xml_metabuf.initialize();

    size_t write_size;
    Menge::TBlobject write_buff(size_test * 2);

    if( xml_metabuf.convert( &write_buff[0], size_test * 2, &buf_test[0], size_test, write_size ) == false )
    {
        char error[512];
        sprintf( error, "writeBin: error\n%s"
            , xml_metabuf.getError().c_str()
            );

        PyErr_SetString( PyToolException, error );

        return NULL;
    }

    FILE * file_test_bin = _wfopen( binPath, L"wb" );

    if( file_test_bin == NULL )
    {
        char error[512];
        sprintf( error, "writeBin: error create bin %ls"
            , binPath
            );

        PyErr_SetString( PyToolException, error );

        return NULL;
    }

    fwrite( &write_buff[0], write_size, 1, file_test_bin );
    fclose( file_test_bin );

    Py_RETURN_NONE;
}
/////////////////////////////////////////////////////////////////////////////////////
static PyMethodDef ModuleMethods[] =
{
    {"writeBin", writeBin, METH_VARARGS, "Greet somebody."},
    {NULL, NULL, 0, NULL}
};
/////////////////////////////////////////////////////////////////////////////////////
struct PyModuleDef module_def = {
    PyModuleDef_HEAD_INIT,
    "ToolsXmlCodecPlugin",
    NULL,
    -1,
    ModuleMethods,
    NULL, NULL, NULL, NULL
};
///////////////////////////////////////////////////////////////////////////////////
PyMODINIT_FUNC PyInit_ToolsXmlCodecPlugin(void)
{
    PyObject * m =  PyModule_Create( &module_def );
    
    if( m == NULL )
    {
        return NULL;
    }
    
    PyToolException = PyErr_NewException("ToolsXmlCodecPlugin.error", NULL, NULL);
        
    Py_INCREF(PyToolException);

    PyModule_AddObject(m, "error", PyToolException);

    return m;
}