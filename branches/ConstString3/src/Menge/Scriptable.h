#	pragma once

//#	include <string>
#	include "Config/Typedef.h"
#	include "pybind/types.hpp"

namespace Menge
{
	class Scriptable
	{
	public:
		Scriptable();
		~Scriptable();

	public:
		void setEmbed( PyObject * _embed );
		PyObject * getEmbed();

	public:
		void incref();
		void decref();

	public:
		void unwrap();

	protected:
		virtual PyObject * _embedded() = 0;

	protected:
		virtual void _embedding( PyObject * );

	protected:
		PyObject * m_embed;
	};
}
