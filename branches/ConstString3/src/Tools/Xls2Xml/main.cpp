#	include <pybind\pybind.hpp>

extern "C"
{
	extern PyObject* PyInit_pyexpat(void);
}

int xlsxExporter(const char* path_data)
{
	pybind::initialize();
	
	PyInit_pyexpat();

	bool exist;

	PyObject* py_xlsExporter = pybind::module_import("xlsxExporter", exist);

	if ( exist == false)
	{
		return 0;
	}

	pybind::call_method(py_xlsExporter, "export", "(s)", path_data);

	return 0;
}