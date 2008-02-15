#	include "Allocator2D.h"

#	include "XmlEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Allocator2D::Allocator2D()
		: m_changePivot( true )
	{
		mt::ident_m3( m_localMatrix );
		mt::ident_m3( m_worldMatrix );
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator2D::changePivot()
	{
		_changePivot();
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator2D::_changePivot()
	{
		m_changePivot = true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Allocator2D::isChangePivot()const
	{
		return m_changePivot;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & Allocator2D::getWorldPosition()
	{
		const mt::mat3f &wm = getWorldMatrix();

		return wm.v2.v2;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & Allocator2D::getWorldDirection()
	{
		const mt::mat3f &wm = getWorldMatrix();

		return wm.v0.v2;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::mat3f & Allocator2D::getWorldMatrix()
	{
		return m_worldMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & Allocator2D::getLocalPosition()const
	{
		return m_localMatrix.v2.v2;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & Allocator2D::getLocalDirection()const
	{
		return m_localMatrix.v0.v2;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::mat3f & Allocator2D::getLocalMatrix()const
	{
		return m_localMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec2f & Allocator2D::getLocalPositionModify()
	{
		return m_localMatrix.v2.v2;
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec2f & Allocator2D::getLocalDirectionModify()
	{
		return m_localMatrix.v0.v2;
	}
	//////////////////////////////////////////////////////////////////////////
	mt::mat3f & Allocator2D::getLocalMatrixModify()
	{
		return m_localMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator2D::setLocalMatrix( const mt::mat3f & _matrix )
	{
		m_localMatrix = _matrix;

		changePivot();
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator2D::setLocalPosition( const mt::vec2f & _position )
	{
		mt::vec2f & localPosition = getLocalPositionModify();

		localPosition = _position;

		changePivot();
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator2D::setLocalDirection( const mt::vec2f & _direction )
	{
		m_localMatrix.v0.v2 = _direction;
		m_localMatrix.v1.v2 = mt::perp( _direction );

		changePivot();
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator2D::setRotate( float _alpha )
	{
		float cos_alpha = cosf(_alpha);
		float sin_alpha = sinf(_alpha);
		m_localMatrix[0][0] = cos_alpha;
		m_localMatrix[0][1] = -sin_alpha;
		m_localMatrix[1][0] = sin_alpha;
		m_localMatrix[1][1] = cos_alpha;
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator2D::translate( const mt::vec2f & _delta )
	{
		m_localMatrix.v2.v2 += _delta;

		changePivot();
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator2D::updateMatrix( Allocator2D * _parent )
	{
		if( m_changePivot == false )
		{
			return;
		}
	
		const mt::mat3f & parentMatrix =
			_parent->getWorldMatrix();
	
		mt::mul_m3_m3( m_worldMatrix, m_localMatrix, parentMatrix );

		_updateMatrix( _parent );
	
		m_changePivot = false;
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator2D::_updateMatrix( Allocator2D * _parent )
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void Allocator2D::loader( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE("Transformation")
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE_MEMBER( "Value", &Allocator2D::setLocalMatrix );
					XML_CASE_ATTRIBUTE_MEMBER( "Position", &Allocator2D::setLocalPosition );
					XML_CASE_ATTRIBUTE_MEMBER( "Direction", &Allocator2D::setLocalDirection );
					XML_CASE_ATTRIBUTE_MEMBER( "Rotate", &Allocator2D::setRotate );
				}
			}
		}
	}
}