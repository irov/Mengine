#	pragma once

#	include "InputHandler.h"

#	include "ActionScheduler.h"

#	include "Actor3D.h"

namespace Menge
{
	class Action;

	class Avatar3D
		: public Actor3D
		, public InputHandler
	{
		OBJECT_DECLARE(Avatar3D)

	public:
		Avatar3D();
	
	public:
		bool handleKeyEvent( size_t _key, bool _isDown ) override;
		bool handleMouseButtonEvent( size_t _button, bool _isDown ) override;
		bool handleMouseMove( float _x, float _y, float _whell ) override;
	public:
		void moveToPoint(const mt::vec3f& _dest);
		void lookAtPoint(const mt::vec3f& _dest);

	public:
		void loader( TiXmlElement * _xml ) override;

	protected:
		void _update( float _timing ) override;
		bool _activate() override;
		void _deactivate() override;
		void _render( const mt::mat4f & _rwm, const Camera3D * _camera ) override;

	private:
		ActionScheduler	m_actionScheduler;
	};
}