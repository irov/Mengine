#	pragma once

#	include "math/mat3.h"

class TiXmlElement;

namespace Menge
{
	class Allocator2D
	{
	public:
		Allocator2D();

	public:
		virtual const mt::mat3f & getWorldMatrix();
		virtual void updateMatrix( Allocator2D * _parent );

		const mt::vec2f & getWorldPosition();
		const mt::vec2f & getWorldDirection();

		const mt::vec2f & getLocalPosition()const;
		const mt::vec2f & getLocalDirection()const;
		const mt::mat3f & getLocalMatrix()const;

		mt::vec2f & getLocalPositionModify();
		mt::vec2f & getLocalDirectionModify();
		mt::mat3f & getLocalMatrixModify();

		void setLocalPosition( const mt::vec2f &position );
		void setLocalDirection( const mt::vec2f &direction );
		
		void translate( const mt::vec2f &delta );

	public:
		void loader( TiXmlElement * _xml );

	public:
		virtual void changePivot();
		bool isChangePivot()const;

	protected:
		virtual void _changePivot();
		virtual void _updateMatrix( Allocator2D * _parent );

	protected:
		mt::mat3f m_localMatrix;
		mt::mat3f m_worldMatrix;

		bool m_changePivot;
	};
}