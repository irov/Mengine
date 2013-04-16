#	pragma once

#	include "Kernel/Node.h"

namespace Menge
{
	class Movie;

	class MovieInternalObject
		: public Node
	{
	public:
		MovieInternalObject();
		~MovieInternalObject();

	public:
		void setMovie( Movie * _movie );
		Movie * getMovie() const;

		void setInternalObject( PyObject * _internalObject );
		PyObject * getInternalObject() const;

	protected:
		bool _activate() override;
		void _deactivate() override;

	protected:
		bool _destroy() override;
		
	protected:
		//void _invalidateWorldMatrix() override;
		//void _invalidateColor() override;

	protected:
		void _localHide( bool _hide ) override;

	protected:
		Movie * m_movie;
		PyObject * m_internalObject;
	};
}