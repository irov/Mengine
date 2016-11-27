#	pragma once

#	include "Interface/ModuleInterface.h"

namespace Menge
{
	class ModuleBase
		: public ModuleInterface
	{
	public:
		ModuleBase();
		~ModuleBase();

	protected:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
		ServiceProviderInterface * getServiceProvider() const override;

	protected:
		void setName( const ConstString & _name ) override;
		const ConstString & getName() const override;

	private:
		bool initialize() override;
		void finalize() override;

	protected:
		virtual bool _initialize();
		virtual void _finalize();

	protected:
		void update( bool _focus ) override;

	protected:
		virtual void _update( bool _focus );

	protected:
		void tick( float _time, float _timing ) override;
		void render( const RenderObjectState * _state, unsigned int _debugMask ) override;

	protected:
		virtual void _tick( float _time, float _timing );
		virtual void _render( const RenderObjectState * _state, unsigned int _debugMask );
		
	protected:
		void message( const ConstString & _messageName, const TMapParams & _params ) override;
		void messageAll (const ConstString & _messageName, const TMapParams & _params) override;

	protected:
		virtual void _message( const ConstString & _messageName, const TMapParams & _params );
		virtual void _messageAll( const ConstString & _messageName, const TMapParams & _params );

	protected:
		ServiceProviderInterface * m_serviceProvider;

		ConstString m_name;
	};
}