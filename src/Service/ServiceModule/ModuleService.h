#pragma once

#include "Interface/ModuleInterface.h"

#include "Core/ServiceBase.h"

#include "stdex/stl_map.h"
#include "stdex/stl_vector.h"

namespace Mengine
{
    class ModuleService
        : public ServiceBase<ModuleServiceInterface>
    {
    public:
        ModuleService();
        ~ModuleService() override;

	public:
		bool _initialize() override;
		void _finalize() override;

	public:
		bool registerModule( const ConstString & _name, const ModuleFactoryInterfacePtr & _module ) override;
		void unregisterModule( const ConstString & _name ) override;

	public:
		bool runModule( const ConstString & _name ) override;
		void stopModule( const ConstString & _name ) override;

	public:
		void update( bool _focus ) override;

	public:
		void tick( float _time, float _timing ) override;
		void render( const RenderState * _state, uint32_t _debugMask ) override;

	public:
		void message( const ConstString & _moduleName, const ConstString & _messageName, const TMapWParams & _params ) override;
		void messageAll( const ConstString & _messageName, const TMapWParams & _params ) override;

	protected:
		const ModuleInterfacePtr & findModule( const ConstString & _moduleName ) const;

    protected:
		typedef stdex::map<ConstString, ModuleFactoryInterfacePtr> TMapModuleFactory;
        TMapModuleFactory m_moduleFactory;

		typedef stdex::vector<ModuleInterfacePtr> TVectorModules;
		TVectorModules m_modules;
    };
}