#	pragma once

#	include "Node.h"

namespace Menge
{
	class ResourceMovie;

	class Movie
		: public Node
	{
	public:
		Movie();

	public:
		void setResourceMovie( const ConstString & _resourceName );
		const ConstString & getResourceMovie() const;

	protected:
		void loader( BinParser * _parser );

	protected:
		bool _compile() override;
		void _release() override;
	
	protected:
		ConstString m_resourceMovieName;
		
		ResourceMovie * m_resourceMovie;
	};
}