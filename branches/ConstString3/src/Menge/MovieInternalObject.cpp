#	include "MovieInternalObject.h"

#	include "Movie.h"

#	include <pybind/pybind.hpp>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MovieInternalObject::MovieInternalObject()
		: m_internalObject(NULL)
		, m_movie(NULL)
	{

	}
	////
	MovieInternalObject::~MovieInternalObject()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void MovieInternalObject::setMovie( Movie * _movie )
	{
		m_movie = _movie;
	}
	//////////////////////////////////////////////////////////////////////////
	Movie * MovieInternalObject::getMovie()
	{
		return m_movie;
	}
	//////////////////////////////////////////////////////////////////////////
	void MovieInternalObject::setInternalObject( PyObject * _internalObject )
	{
		m_internalObject = _internalObject;

		pybind::incref( m_internalObject );
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * MovieInternalObject::getInternalObject() const
	{
		return m_internalObject;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MovieInternalObject::_activate()
	{
		if( Node::_activate() == false )
		{
			return false;
		}

		if( m_internalObject != NULL )
		{
			PyObject * py_obj = this->getEmbed();
		
			m_movie->callEvent( EVENT_MOVIE_ACTIVATE_INTERNAL, "(OO)", py_obj, m_internalObject );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void MovieInternalObject::_deactivate()
	{
		Node::_deactivate();
				
		if( m_internalObject != NULL )
		{
			PyObject * py_obj = this->getEmbed();

			m_movie->callEvent( EVENT_MOVIE_DEACTIVATE_INTERNAL, "(OO)", py_obj, m_internalObject );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void MovieInternalObject::_destroy()
	{
		Node::_destroy();

		pybind::decref( m_internalObject );
		m_internalObject = NULL;
	}
	//////////////////////////////////////////////////////////////////////////		
	//void MovieInternalObject::_invalidateWorldMatrix()
	//{
	//	this->_invalidateWorldMatrix();

	//	const mt::vec3f & anchorPoint = this->getOrigin();
	//	const mt::vec3f & position = this->getLocalPosition();
	//	const mt::vec3f & scale = this->getScale();
	//	const mt::vec3f & rotation = this->getRotation();

	//	PyObject * py_anchorPoint = pybind::ptr(anchorPoint);
	//	PyObject * py_position = pybind::ptr(position);
	//	PyObject * py_scale = pybind::ptr(scale);
	//	PyObject * py_rotation = pybind::ptr(rotation);

	//	m_movie->callEvent( EVENT_MOVIE_APPLY_INTERNAL_TRANSFORMATION, "(OOOOO)", m_internalObject, py_anchorPoint, py_position, py_scale, py_rotation );

	//	pybind::decref(py_anchorPoint);
	//	pybind::decref(py_position);
	//	pybind::decref(py_scale);
	//	pybind::decref(py_rotation);
	//}
	//////////////////////////////////////////////////////////////////////////
	//void MovieInternalObject::_invalidateColor()
	//{
	//	this->_invalidateColor();

	//	ColourValue colour;
	//	this->calcTotalColor( colour );

	//	float opacity = colour.getA();

	//	PyObject * py_opacity = pybind::ptr(opacity);

	//	m_movie->callEvent( EVENT_MOVIE_APPLY_INTERNAL_OPACITY, "(OO)", m_internalObject, py_opacity );

	//	pybind::decref(py_opacity);
	//}
}