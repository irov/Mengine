#	pragma once

#	include "math/mat4.h"

class XmlElement;

namespace Menge
{
	class Allocator3D_
	{
	public:
		Allocator3D_();

	public:
		virtual const mt::vec3f & getWorldPosition();
		virtual const mt::vec3f & getWorldDirection();
		virtual const mt::vec3f & getWorldStrafe();
		virtual const mt::mat4f & getWorldMatrix();

		const mt::vec3f & getLocalPosition()const;
		mt::vec3f & getLocalPosition();

		const mt::vec3f & getLocalDirection()const;
		mt::vec3f & getLocalDirection();

		const mt::vec3f & getLocalStrafe()const;
		mt::vec3f & getLocalStrafe();

		const mt::mat4f & getLocalMatrix()const;
		mt::mat4f & getLocalMatrix();

		void setLocalPosition( const mt::vec3f &position );
		void setDirection( const mt::vec3f &direction );
		
		void translate( const mt::vec3f &delta );

	public:
		void loader( XmlElement * _xml );

	public:
		void updateMatrix( Allocator3D_ * _parent );

		virtual void changePivot();
		bool isChangePivot()const;

	protected:
		virtual void _changePivot();
		virtual void _updateMatrix( Allocator3D_ * _parent );

	protected:
		mt::mat4f m_localMatrix;
		mt::mat4f m_worldMatrix;

		mt::vec3f m_fixedUp;

		bool m_changePivot;
	};
}