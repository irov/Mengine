#	pragma once

#	include "math/mat3.h"

class XmlElement;

namespace Menge
{
	class Allocator2DChecker
	{
	public:
		virtual bool check( mt::mat3f & _matrix ) = 0;
	};

	class Allocator2D
	{
	public:
		Allocator2D();

	public:
		const mt::mat3f & getWorldMatrix() const;

	public:
		const mt::vec2f & getLocalPosition() const;
		const mt::vec2f & getLocalDirection() const;
		const mt::mat3f & getLocalMatrix() const;

		void setLocalMatrix( const mt::mat3f & _matrix );
		void setLocalPosition( const mt::vec2f & _position );
		void setLocalDirection( const mt::vec2f & _direction );

		void setRotate( float _alpha );
		
		void translate( const mt::vec2f & _delta );

	public:
		const mt::mat3f & updateWorldMatrix( const mt::mat3f & _parentMatrix );

	public:
		void loader( XmlElement * _xml );

	public:
		virtual void changePivot();
		bool isChangePivot() const;

	public:
		void setChecker( Allocator2DChecker * _checker );

	protected:
		virtual void _changePivot();
		virtual void _updateWorldMatrix( const mt::mat3f & _parentMatrix );

	protected:
		mt::mat3f m_localMatrix;
		mt::mat3f m_worldMatrix;

		bool m_fixedRotation;
		bool m_changePivot;

		Allocator2DChecker * m_checker;
	};
}