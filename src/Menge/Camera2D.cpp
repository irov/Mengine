#	include "Camera2D.h"

#	include "ObjectImplement.h"

#	include "XmlParser/XmlParser.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT( Camera2D );
	//////////////////////////////////////////////////////////////////////////
	Camera2D::Camera2D()
	{}
	//////////////////////////////////////////////////////////////////////////
	Camera2D::~Camera2D()
	{}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::loader( TiXmlElement * _xml )
	{
		Allocator2D::loader( _xml );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Camera2D::_activate()
	{
		return true;
	};
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::_update( size_t _timing )
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::_changePivot()
	{
		Allocator2D::_changePivot();

		updateViewport();
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::updateViewport()
	{
		const mt::vec2f & pos = getWorldPosition();

		m_viewport.begin = pos - m_viewportSize * .5;
		m_viewport.end = pos + m_viewportSize * .5;
	}
	//////////////////////////////////////////////////////////////////////////
	const Viewport & Camera2D::getViewport() const
	{
		return m_viewport;
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera2D::setViewportSize( const mt::vec2f & _size )
	{
		m_viewportSize = _size;
		updateViewport();
	}
}