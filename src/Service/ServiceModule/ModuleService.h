#pragma once

#include "Interface/ModuleInterface.h"

#include "Core/ServiceBase.h"

#include "Config/Map.h"
#include "Config/Vector.h"

namespace Mengine
{
    class ModuleService
        : public ServiceBase<ModuleServiceInterface>
    {
    public:
        ModuleService();
        ~ModuleService() override;

	public:
		bool _initializeService() override;
		void _finalizeService() override;

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
		void render( const RenderContext * _state, uint32_t _debugMask ) override;

	public:
		void message( const ConstString & _moduleName, const ConstString & _messageName, const TMapWParams & _params ) override;
		void messageAll( const ConstString & _messageName, const TMapWParams & _params ) override;

	protected:
		const ModuleInterfacePtr & findModule( const ConstString & _moduleName ) const;

    protected:
		typedef Map<ConstString, ModuleFactoryInterfacePtr> TMapModuleFactory;
        TMapModuleFactory m_moduleFactory;

		typedef Vector<ModuleInterfacePtr> TVectorModules;
		TVectorModules m_modules;
    };
}