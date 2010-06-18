#	pragma once

//#	include <string>
#	include "Config/Typedef.h"

extern "C" 
{ 
	struct _object; 
	typedef _object PyObject;
}

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

	public:
		bool decrefEmbedding();
		void increfEmbedding();

	protected:
		virtual PyObject * _embedded() = 0;

	public:
		void callMethod( const ConstString& _method, const char * _format, ... );

	protected:
		PyObject * m_embedding;
	};
}
