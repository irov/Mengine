#	pragma once

#	include "Interface/PluginInterface.h"
#	include "Interface/ModuleInterface.h"

#   include "Core/ServantBase.h"

namespace Menge
{
	class PluginBase
		: public ServantBase<PluginInterface>
	{
	public:
		PluginBase();
		~PluginBase();

    protected:
		void setDynamicLoad( bool _dynamicLoad ) override;
		bool isDynamicLoad() const override;
		
    protected:
        bool avaliable() override;

    protected:
        virtual bool _avaliable();

    protected:
		bool initialize() override;
		void finalize() override;

	protected:
		bool isInitialize() const override;

	protected:
		void destroy() override;

	protected:
		virtual bool _initialize();
		virtual void _finalize();

	protected:
		bool addModuleFactory(const ConstString & _name, const ModuleFactoryInterfacePtr & _factory);

	protected:
		bool m_dynamicLoad;
		bool m_initialize;

		typedef stdex::vector<ConstString> TVectorModuleFactory;
		TVectorModuleFactory m_moduleFactories;
	};
}