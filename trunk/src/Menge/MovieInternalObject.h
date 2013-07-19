#	pragma once

#	include "Kernel/Node.h"

#   include "ResourceInternalObject.h"

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

		void setResourceInternalObject( ResourceInternalObject * _resource );
		ResourceInternalObject * getResourceInternalObject() const;

	protected:
        bool _compile() override;
        void _release() override;

		bool _activate() override;
		void _deactivate() override;
        
	protected:
		bool _destroy() override;
		
	protected:
		void _localHide( bool _hide ) override;

	protected:
        ResourceHolder<ResourceInternalObject> m_resourceInternalObject;
        Movie * m_movie;

		PyObject * m_internalObject;
	};
}