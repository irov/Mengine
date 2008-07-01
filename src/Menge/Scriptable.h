#	pragma once

#	include <string>

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

	protected:
		virtual PyObject * _embedded() = 0;

	public:
		void callMethod( const std::string & _method, const char * _format, ... );

	protected:
		PyObject * m_embedding;
	};
}