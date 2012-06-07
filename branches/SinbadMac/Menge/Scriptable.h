#	pragma once

//#	include <string>
#	include "Config/Typedef.h"

#	include <pybind/types.hpp>

namespace Menge
{
	class Scriptable
	{
	public:
		Scriptable();
		~Scriptable();

	public:
		PyObject * getEmbedding();
		void setEmbedding( PyObject * _embedding );

	protected:
		virtual PyObject * _embedded() = 0;

	public:
		void callMethod( const String& _method, const char * _format, ... );

	protected:
		PyObject * m_embedding;
	};
}
