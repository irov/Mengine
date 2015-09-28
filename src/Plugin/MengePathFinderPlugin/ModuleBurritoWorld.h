#	pragma once

#	include "Interface/ModuleInterface.h"

#	include "BurritoWorld.h"

namespace Menge
{
	class ModuleBurritoWorld
		: public ModuleInterface
	{
	public:
		ModuleBurritoWorld();
		~ModuleBurritoWorld();

	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
		ServiceProviderInterface * getServiceProvider() const override;

	public:
		bool initialize() override;
		void finalize() override;

		void setName( const ConstString & _name ) override;
		const ConstString & getName() const override;

	public:
		BurritoWorld * createBurritoWorld();
		void removeBurritoWorld( BurritoWorld * _world );

	public:
		void update( float _time, float _timing ) override;
		void render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, const RenderClipplaneInterface * _clipplane, unsigned int _debugMask ) override;
		
	protected:
		ServiceProviderInterface * m_serviceProvider;
		ConstString m_name;

		typedef stdex::vector<BurritoWorld *> TVectorBurritoWorld;
		TVectorBurritoWorld m_worlds;
		TVectorBurritoWorld m_worldsAdd;
	};
}