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
		void setEmbed( PyObject * _embed );
		PyObject * getEmbed();

	protected:
		virtual PyObject * _embedded() = 0;

	protected:
		virtual void _embedding( PyObject * );

	protected:
		PyObject * m_embed;
	};
}
