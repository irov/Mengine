#pragma once

#include "Interface/ModuleInterface.h"

#include "Kernel/ServantBase.h"

namespace Mengine
{
	class ModuleBase
		: public ServantBase<ModuleInterface>
	{
	public:
		ModuleBase();
		~ModuleBase() override;

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
		void tick( const UpdateContext * _context ) override;
		void render( const RenderContext * _state ) override;

	protected:
		virtual void _tick( const UpdateContext * _context );
		virtual void _render( const RenderContext * _state );
		
	protected:
		void message( const ConstString & _messageName, const MapWParams & _params ) override;
		void messageAll (const ConstString & _messageName, const MapWParams & _params) override;

	protected:
		virtual void _message( const ConstString & _messageName, const MapWParams & _params );
		virtual void _messageAll( const ConstString & _messageName, const MapWParams & _params );

	protected:
		ConstString m_name;
	};
}