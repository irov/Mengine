#	pragma once

#	include "Node.h"

namespace Menge
{
	class Movie;

	class MovieInternalObject
		: public Node
	{
	public:
		MovieInternalObject();

	public:
		void setMovie( Movie * _movie );
		Movie * getMovie();

		void setInternalObject( PyObject * _internalObject );
		PyObject * getInternalObject() const;

	protected:
		bool _activate() override;
		void _deactivate() override;

	protected:
		void _destroy() override;
		

	protected:
		void _invalidateWorldMatrix() override;
		void _invalidateColor() override;

	protected:
		Movie * m_movie;
		PyObject * m_internalObject;
	};
}