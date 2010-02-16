#	include "pyCallback.h"
//////////////////////////////////////////////////////////////////////////
pyCallback::pyCallback(PyObject * _pyObject)
	: m_pyObject( _pyObject )
{}
//////////////////////////////////////////////////////////////////////////
pyCallback::~pyCallback()
{
	// delete ? m_pyObject
}
//////////////////////////////////////////////////////////////////////////
bool pyCallback::execute( const std::string & _src, const std::string & _dst, Compiler * )
{
	pybind::call( m_pyObject, "(ss)", _src.c_str(), _dst.c_str() );
	return true;
}