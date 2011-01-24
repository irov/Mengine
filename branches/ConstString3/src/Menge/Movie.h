#	pragma once

#	include "Node.h"

namespace Menge
{
	class Sprite;
	class ResourceMovie;

	class Movie
		: public Node
	{
	public:
		Movie();

	public:
		void setResourceMovie( const ConstString & _resourceName );
		const ConstString & getResourceMovie() const;

	public:
		void run();
		void stop();

	protected:
		void loader( BinParser * _parser );

	protected:
		bool _compile() override;
		void _release() override;

	protected:
		void _update( float _timing ) override;
	
	protected:
		ConstString m_resourceMovieName;
		
		ResourceMovie * m_resourceMovie;

		typedef std::vector<Sprite *> TVectorSprites;
		TVectorSprites m_sprites;

		float m_timming;
		bool m_play;
	};
}