#	pragma once

#	include "math/mat4.h"

class TiXmlElement;

namespace Menge
{
	class Allocator3D
	{
	public:
		Allocator3D();

	public:
		const mt::vec3f & getWorldPosition();
		const mt::vec3f & getWorldDirection();
		const mt::mat4f & getWorldMatrix();

		const mt::vec3f & getLocalPosition()const;
		mt::vec3f & getLocalPosition();

		const mt::vec3f & getLocalDirection()const;
		mt::vec3f & getLocalDirection();

		const mt::mat4f & getLocalMatrix()const;
		mt::mat4f & getLocalMatrix();

		void setPosition( const mt::vec3f &position );
		void setDirection( const mt::vec3f &direction );
		
		void translate( const mt::vec3f &delta );

	public:
		void loader( TiXmlElement * _xml );

	public:
		void updateMatrix( Allocator3D * _parent );

		void changePivot();
		bool isChangePivot()const;

	protected:
		virtual void _changePivot();
		virtual bool _updateMatrix();

	protected:
		mt::mat4f m_localMatrix;
		mt::mat4f m_worldMatrix;

		mt::vec3f m_fixedUp;

		bool m_changePivot;
	};
}