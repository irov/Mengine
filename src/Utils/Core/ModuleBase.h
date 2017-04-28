#	pragma once

#	include "Interface/ModuleInterface.h"

#   include "Core/ServantBase.h"

namespace Menge
{
	class ModuleBase
		: public ServantBase<ModuleInterface>
	{
	public:
		ModuleBase();
		~ModuleBase();

	protected:
		void setName( const ConstString & _name ) override;
		const ConstString & getName() const override;

	protected:
		bool avaliable() override;

	protected:
		virtual bool _avaliable();

	protected:
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
		void render( const RenderObjectState * _state, uint32_t _debugMask ) override;

	protected:
		virtual void _tick( float _time, float _timing );
		virtual void _render( const RenderObjectState * _state, uint32_t _debugMask );
		
	protected:
		void message( const ConstString & _messageName, const TMapWParams & _params ) override;
		void messageAll (const ConstString & _messageName, const TMapWParams & _params) override;

	protected:
		virtual void _message( const ConstString & _messageName, const TMapWParams & _params );
		virtual void _messageAll( const ConstString & _messageName, const TMapWParams & _params );

	protected:
		ConstString m_name;
	};
}