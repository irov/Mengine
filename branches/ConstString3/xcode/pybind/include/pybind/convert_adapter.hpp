#	pragma once

#	include "pybind/adapter_interface.hpp"

namespace pybind
{
	class convert_adapter_interface
		: public adapter_interface
	{
	public:
		virtual bool convert( PyObject * _obj, void * _place ) = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	template<class F>
	class convert_adapter
		: public convert_adapter_interface
	{
	public:
		convert_adapter( F _convert )
			: m_convert(_convert)
		{
		}

	public:
		virtual bool convert( PyObject * _obj, void * _place ) override
		{
			bool result = (*m_convert)( _obj, _place );

			return result;
		}

	protected:
		F m_convert;
	};
}

