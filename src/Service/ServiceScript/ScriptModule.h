#	pragma once

#	include "Interface/ScriptSystemInterface.h"
#	include "Interface/LoggerInterface.h"

#	include "Kernel/Servant.h"

#	include "pybind/types.hpp"
#	include "pybind/pybind.hpp"

namespace Menge
{
	class ScriptModule
		: public ScriptModuleInterface
		, public Servant
	{
	public:
		ScriptModule();

	public:
		bool initialize( const pybind::module & _module );

	protected:
		bool onInitialize( const ConstString & _method ) override;
		bool onFinalize( const ConstString & _method ) override;

    protected:
        const pybind::base & getModule() const override;

	protected:
		pybind::module m_module;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<ScriptModule> ScriptModulePtr;
}