#	include "Callback.h"

#	include "pybind/pybind.hpp"

class pyCallback : public Callback
{
public:
	pyCallback( PyObject * _pyObject );
	~pyCallback();
public:
	bool execute( const std::string & _src, const std::string & _dst, Compiler * );
private:
	PyObject * m_pyObject;
};