#	pragma once

#	include "Node.h"
#	include "Animatable.h"

namespace Menge
{
	class Sprite;
	class ResourceMovie;

	class Movie
		: public Node
		, public Animatable
	{
	public:
		Movie();

	public:
		void setResourceMovie( const ConstString & _resourceName );
		const ConstString & getResourceMovie() const;
		
	public:
		void setFirstFrame();
		void setLastFrame();

	protected:
		bool _play() override;
		void _stop( std::size_t _enumerator ) override;
		void _end( std::size_t _enumerator ) override;

	protected:
		void loader( BinParser * _parser );

	protected:
		bool _compile() override;
		void _release() override;

	protected:
		bool _activate() override;

	protected:
		void _update( float _timing ) override;

	protected:
		void _setEventListener( PyObject * _embed ) override;

	private:
		void activateLayer_( std::size_t _index ) const;
		void updateParent_();
	
	protected:
		ConstString m_resourceMovieName;
		
		ResourceMovie * m_resourceMovie;

		typedef std::map<size_t, Node *> TMapNode;
		TMapNode m_nodies;

		typedef std::map<size_t, Sprite *> TMapFlexSprite;
		TMapFlexSprite m_flexSprites;

		float m_timing;
		float m_out;
	};
}