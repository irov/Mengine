#   pragma once

#   include "Interface/ModuleInterface.h"

#   include <map>

namespace Menge
{
    class ModuleService
        : public ModuleServiceInterface
    {
    public:
        ModuleService();
        ~ModuleService();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

	public:
		bool registerModule( const ConstString & _name, const ModuleFactoryInterfacePtr & _module ) override;
		void unregisterModule( const ConstString & _name ) override;

	public:
		bool runModule( const ConstString & _name ) override;
		void stopModule( const ConstString & _name ) override;

    protected:
        ServiceProviderInterface * m_serviceProvider;

		typedef std::map<ConstString, ModuleFactoryInterfacePtr> TMapModuleFactory;
        TMapModuleFactory m_moduleFactory;

		typedef std::map<ConstString, ModuleInterfacePtr> TMapModules;
		TMapModules m_modules;
    };
}