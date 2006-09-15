#	pragma once

#	include "Node.h"

#	include "math/mat3.h"

namespace RvEngine
{
	class Allocator
		: public Node
	{
		OBJECT_DECLARE(Allocator);

	public:
		Allocator();

	public:
		const mt::vec2f & getWorldPosition();
		const mt::vec2f & getWorldDirection();
		const mt::mat3f & getWorldMatrix();

		const mt::vec2f & getLocalPosition()const;
		mt::vec2f & getLocalPosition();

		const mt::vec2f & getLocalDirection()const;
		mt::vec2f & getLocalDirection();

		const mt::mat3f & getLocalMatrix()const;
		mt::mat3f & getLocalMatrix();

		void setPosition( const mt::vec2f &position );
		void setDirection( const mt::vec2f &direction );
		void setWorldMatrix( const mt::mat3f &matrix );

		void translate( const mt::vec2f &delta );

	public:
		bool isChangePivot()const;

	protected:
		virtual void _changePivot();		

	private:	
		void changePivot();
		void updateMatrix();

	public:
		void updateParent()override;

	protected:
		Allocator *m_parentAllocator;

		mt::mat3f m_localMatrix;
		mt::mat3f m_worldMatrix;

		bool m_changePivot;
	};
}