#	pragma once

#	include "NodeImpl.h"

#	include "math/mat3.h"

namespace Menge
{
	class Allocator2D
		: public NodeImpl
	{
		OBJECT_DECLARE(Allocator2D);

	public:
		Allocator2D();

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
		void setParentAllocator( Allocator2D *_alloc);
		Allocator2D * getParentAllocator();

	public:
		void loader( TiXmlElement * _xml ) override;

	public:
		void changePivot();
		bool isChangePivot()const;

	protected:
		virtual void _changePivot();
		virtual bool _updateMatrix();

	protected:
		void _updateParent()override;

	protected:
		Allocator2D *m_parentAllocator;

		mt::mat3f m_localMatrix;
		mt::mat3f m_worldMatrix;

		bool m_changePivot;

	private:
		void updateMatrix();
	};
}