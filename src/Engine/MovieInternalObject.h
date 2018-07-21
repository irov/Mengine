#pragma once

#include "Kernel/Node.h"
#include "Kernel/ResourceHolder.h"

#include "ResourceInternalObject.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
	typedef IntrusivePtr<class Movie> MoviePtr;

	class MovieInternalObject
		: public Node
	{
	public:
		MovieInternalObject();
		~MovieInternalObject() override;

	public:
		void setMovie( const MoviePtr & _movie );
		const MoviePtr & getMovie() const;

		void setResourceInternalObject( const ResourceInternalObjectPtr & _resource );
		const ResourceInternalObjectPtr & getResourceInternalObject() const;

	public:
		const NodePtr & getInternalNode() const;

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
        MoviePtr m_movie;

		pybind::object m_internalObject;
		NodePtr m_internalNode;
	};
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MovieInternalObject> MovieInternalObjectPtr;
    //////////////////////////////////////////////////////////////////////////
}