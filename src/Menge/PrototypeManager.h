#	pragma once

#   include "Interface/PrototypeManagerInterface.h"

#	include "Config/Typedef.h"

#	include "Core/ConstString.h"

#	include "pybind/pybind.hpp"

#	include <map>

namespace Menge
{
	class PrototypeManager
        : virtual public PrototypeManagerInterface
	{
	public:
		PrototypeManager();
		~PrototypeManager();

	public:
		bool addPrototype( const ConstString & _prototype, PyObject * _module ) override;
		PyObject * getPrototype( const ConstString & _prototype ) const override;

	public:
		bool hasPrototype( const ConstString & _prototype, PyObject ** _module ) const override;

	protected:
		typedef std::map<ConstString, PyObject *> TMapPrototypes;
		TMapPrototypes m_prototypes;
	};
}