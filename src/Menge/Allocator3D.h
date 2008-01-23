#	pragma once

#	include "math/mat4.h"

class XmlElement;

namespace Menge
{
	class Allocator3D
	{
	public:
		Allocator3D();

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
		void setLocalDirection( const mt::vec3f &direction );
		void setLocalRotation( const mt::mat3f& M );

		void translate( const mt::vec3f &delta );

	public:
		void loader( XmlElement * _xml );

		void debugRender();

	public:
		void updateMatrix( Allocator3D * _parent );

		virtual void changePivot();
		bool isChangePivot()const;

	protected:
		virtual void _changePivot();
		virtual void _updateMatrix( Allocator3D * _parent );

	protected:
		mt::mat4f m_localMatrix;
		mt::mat4f m_worldMatrix;

		mt::vec3f m_fixedUp;

		bool m_changePivot;
	};
}