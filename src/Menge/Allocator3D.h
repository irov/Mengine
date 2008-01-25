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

		virtual const mt::quatf & getWorldOrient();
		virtual const mt::vec3f & getWorldPosition();

		const mt::vec3f & getLocalPosition()const;
		mt::vec3f & getLocalPosition();

		const mt::quatf & getLocalOrient()const;
		mt::quatf & getLocalOrient();

		void setLocalPosition( const mt::vec3f &position );
		void setLocalOrient( const mt::quatf &quat );

		void translate( const mt::vec3f &delta );

	public:
		void loader( XmlElement * _xml );

		void debugRender();

	public:
		void updateOrient( Allocator3D * _parent );
		void updatePosition( Allocator3D * _parent );

		virtual void changePivot();

	protected:
		virtual void _changePivot();

	protected:
		mt::vec3f m_localPosition;
		mt::vec3f m_worldPosition;

		mt::quatf m_localOrient;
		mt::quatf m_worldOrient;

		bool m_changePivot;
	};
}