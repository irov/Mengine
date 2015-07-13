#	pragma once

#	include "Kernel/Node.h"
#	include "Kernel/Animatable.h"

#	include "spine/spine.h"

namespace Menge
{
	class Spine
		: public Node
		, public Animatable
	{
	public:
		Spine();
		~Spine();

	protected:
		bool _compile() override;
		void _release() override;

	protected:
		void _update( float _current, float _timing ) override;
		void _render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera ) override;

	protected:
		bool _play( float _time ) override;
		bool _restart( float _time, uint32_t _enumerator ) override;
		void _pause( uint32_t _enumerator ) override;
		void _resume( uint32_t _enumerator ) override;
		void _stop( uint32_t _enumerator ) override;
		void _end( uint32_t _enumerator ) override;
		bool _interrupt( uint32_t _enumerator ) override;
	};
}