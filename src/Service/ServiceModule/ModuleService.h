#   pragma once

#   include "Interface/ModuleInterface.h"

#   include <stdex/stl_map.h>
#   include <stdex/stl_vector.h>

namespace Menge
{
    class ModuleService
        : public ServiceBase<ModuleServiceInterface>
    {
    public:
        ModuleService();
        ~ModuleService();

	public:
		bool registerModule( const ConstString & _name, const ModuleFactoryInterfacePtr & _module ) override;
		void unregisterModule( const ConstString & _name ) override;

	public:
		bool runModule( const ConstString & _name ) override;
		void stopModule( const ConstString & _name ) override;

	public:
		void update( float _time, float _timing ) override;
		void render( const RenderObjectState * _state, uint32_t _debugMask ) override;

    protected:
		typedef stdex::map<ConstString, ModuleFactoryInterfacePtr> TMapModuleFactory;
        TMapModuleFactory m_moduleFactory;

		typedef stdex::vector<ModuleInterfacePtr> TVectorModules;
		TVectorModules m_modules;
    };
}