//#	pragma once
//
//#	include "Loadable.h"
//
//#	include "math/mat4.h"
//
//namespace Menge
//{
//	class Allocator3D
//		: virtual public Loadable
//	{
//	public:
//		Allocator3D();
//
//	public:
//		virtual const mt::vec3f & getWorldPosition3D();
//		virtual const mt::vec3f & getWorldDirection3D();
//		virtual const mt::vec3f & getWorldStrafe3D();
//		virtual const mt::mat4f & getWorldMatrix3D();
//
//		const mt::vec3f & getLocalPosition3D()const;
//		//mt::vec3f & getLocalPosition3D();
//
//		const mt::vec3f & getLocalDirection3D()const;
//		//mt::vec3f & getLocalDirection3D();
//
//		const mt::vec3f & getLocalStrafe3D()const;
//		//mt::vec3f & getLocalStrafe3D();
//
//		const mt::mat4f & getLocalMatrix3D()const;
//		//mt::mat4f & getLocalMatrix3D();
//
//		void setLocalPosition3D( const mt::vec3f &position );
//		void setDirection3D( const mt::vec3f &direction );
//		
//		void translate3D( const mt::vec3f &delta );
//
//		void scale( const mt::vec3f& _scale );
//
//	public:
//		void loader( BinParser * _parser ) override;
//
//	public:
//		void updateMatrix3D(  const mt::mat4f & parentMatrix /*Allocator3D * _parent*/ );
//
//		virtual void changePivot3D();
//		bool isChangePivot3D()const;
//
//	protected:
//		virtual void _changePivot3D();
//		virtual void _updateMatrix3D();
//
//	protected:
//		mt::mat4f m_localMatrix3D;
//		mt::mat4f m_worldMatrix3D;
//
//		mt::vec3f m_fixedUp;
//
//		bool m_changePivot3D;
//	};
//}
