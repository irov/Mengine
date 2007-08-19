#	pragma once

#	include "InputHandler.h"

#	include "AnimationObject.h"

#	include "ActionScheduler.h"

namespace Menge
{
	class Action;

	class Avatar3D
		: public AnimationObject
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
		void loader( TiXmlElement * _xml ) override;

	protected:
		void _update( float _timing ) override;
	public:
		void moveTo(const mt::vec3f& _dest);
		void lookTo(const mt::vec3f& _target);
		void stop();
		void moveToPoint(const mt::vec3f& _dest);
		void calculateNewPos( float _timing );
		void calculateDestPos(const mt::vec3f& _dest);
		bool isMoving() const;
	private:
		bool		m_isMove;
		mt::vec3f	m_dir;
		mt::vec3f   m_destpos;
		float		m_velocity;

		ActionScheduler	m_actionScheduler;
	};
}