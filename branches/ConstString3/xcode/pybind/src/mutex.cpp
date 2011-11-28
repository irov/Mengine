#	include "pybind/mutex.hpp"
#	include "config/config.hpp"
#	include "config/python.hpp"

namespace pybind
{
	mutex::mutex()
	{
		//m_state = (int)PyGILState_Ensure();
	}
		
	mutex::~mutex()
	{
		//PyGILState_Release( (PyGILState_STATE) m_state);
	}
}
