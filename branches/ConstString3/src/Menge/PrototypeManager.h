#	pragma once

#	include "Config/Typedef.h"

#	include "Core/ConstString.h"

#	include "pybind/pybind.hpp"

#	include <map>

namespace Menge
{
	class PrototypeManager
	{
	public:
		PrototypeManager();
		~PrototypeManager();

	public:
		bool addPrototype( const ConstString & _prototype, PyObject * _module );
		PyObject * getPrototype( const ConstString & _prototype ) const;

	public:
		bool hasPrototype( const ConstString & _prototype ) const;

	protected:
		typedef std::map<ConstString, PyObject *> TMapPrototypes;
		TMapPrototypes m_prototypes;
	};
}