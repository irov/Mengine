#	pragma once

#	include "Interface/ScriptSystemInterface.h"

namespace Menge
{
	class PythonScriptSystem
		: public ScriptSystemInterface
	{
	public:
		bool initialize() override;
		void finalize() override;

	public:
		ScriptTypeInterface * registerClass( const char * _name, const std::type_info & _info ) override;
	};
}