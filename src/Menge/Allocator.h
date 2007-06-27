//#	pragma once
//
//#	include "NodeImpl.h"
//
//#	include "math/mat4.h"
//
//namespace Menge
//{
//	class Allocator
//		: public NodeImpl
//	{
//		OBJECT_DECLARE(Allocator);
//
//	public:
//		Allocator();
//
//	public:
//		const mt::vec3f & getWorldPosition();
//		const mt::vec3f & getWorldDirection();
//		const mt::mat4f & getWorldMatrix();
//
//		const mt::vec3f & getLocalPosition()const;
//		mt::vec3f & getLocalPosition();
//
//		const mt::vec3f & getLocalDirection()const;
//		mt::vec3f & getLocalDirection();
//
//		const mt::mat4f & getLocalMatrix()const;
//		mt::mat4f & getLocalMatrix();
//
//		void setPosition( const mt::vec3f &position );
//		void setDirection( const mt::vec3f &direction );
//		
//		void translate( const mt::vec3f &delta );
//
//	public: 
//		void setParentAllocator( Allocator *_alloc);
//		Allocator * getParentAllocator();
//
//	public:
//		void loader( TiXmlElement * _xml ) override;
//
//	public:
//		void changePivot();
//		bool isChangePivot()const;
//
//	protected:
//		virtual void _changePivot();
//		virtual bool _updateMatrix();
//
//	protected:
//		void _updateParent()override;
//
//	protected:
//		Allocator *m_parentAllocator;
//
//		mt::mat4f m_localMatrix;
//		mt::mat4f m_worldMatrix;
//
//		bool m_changePivot;
//
//	private:
//		void updateMatrix();
//	};
//}