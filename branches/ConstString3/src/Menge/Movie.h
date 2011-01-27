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
		void setAutoPlay( bool _value );

	public:
		void play();
		void stop();

	protected:
		void loader( BinParser * _parser );

	protected:
		bool _compile() override;
		void _release() override;

	protected:
		bool _activate() override;

	protected:
		void _update( float _timing ) override;

	public:
		void activateLayer_( int i );
	
	protected:
		ConstString m_resourceMovieName;
		
		ResourceMovie * m_resourceMovie;

		typedef std::vector<Sprite *> TVectorSprites;
		TVectorSprites m_sprites;

		float m_lasting;
		float m_timing;

		bool m_autoPlay;

		bool m_play;
	};
}