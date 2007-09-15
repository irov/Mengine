#	pragma once

#	include "InputHandler.h"

#	include "StackFSM.h"

#	include "Actor2D.h"

#	include <vector>

namespace Menge
{
	class State;

	class Avatar2D
		: public Actor2D
		, public InputHandler
	{
		OBJECT_DECLARE(Avatar2D)

	public:
		Avatar2D();
	
	public:
		bool handleKeyEvent( size_t _key, bool _isDown ) override;
		bool handleMouseButtonEvent( size_t _button, bool _isDown ) override;
		bool handleMouseMove( float _x, float _y, float _whell ) override;
	public:
		void moveToPoint( const mt::vec2f& _dest );
		void lookAtPoint( const mt::vec2f& _dest );
		void moveByWayPoints( const std::vector<mt::vec2f>& _wayPoints );

	public:
		void loader( TiXmlElement * _xml ) override;

	protected:
		void _update( float _timing ) override;
		bool _activate() override;
		void _deactivate() override;
		void _render( const mt::mat3f & _rwm, const Viewport & _viewport ) override;

	private:
		StackFSM	m_stackFSM;
	};
}