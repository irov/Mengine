#pragma once

#include "Kernel/Node.h"

#   include "ResourceInternalObject.h"

#   include "pybind/pybind.hpp"

namespace Mengine
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

		void setResourceInternalObject( const ResourceInternalObjectPtr & _resource );
		const ResourceInternalObjectPtr & getResourceInternalObject() const;

	public:
		Node * getInternalNode() const;

	protected:
        bool _compile() override;
        void _release() override;

		bool _activate() override;
		void _deactivate() override;
        	
	protected:
		void _setPersonalColor( const ColourValue& _color ) override;
		void _setPersonalAlpha( float _alpha ) override;

	protected:
		void _setLocalHide( bool _hide ) override;

	protected:
        ResourceHolder<ResourceInternalObject> m_resourceInternalObject;
        Movie * m_movie;

		pybind::object m_internalObject;
		Node * m_internalNode;
	};
}