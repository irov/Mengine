#	pragma once

#	include "Node.h"
#	include "Animatable.h"

namespace Menge
{
	class Sprite;
	class Animation;

	struct MovieLayer2D;
	struct MovieFrame2D;

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

		void setReverse( bool _value);
		bool getReverse() const;

	protected:
		bool _play() override;
		bool _restart( size_t _enumerator ) override;
		void _stop( size_t _enumerator ) override;
		void _end( size_t _enumerator ) override;

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
		void updateFrame2D_( const MovieLayer2D & layer, Node * _node, const MovieFrame2D & _frame, const mt::mat3f & _wm );

	private:
		void updateParent_();
		void playAnimation_();

	private:
		Scriptable * findInternalObject_( const ConstString & _resource, EEventName _event );
	
	protected:
		ConstString m_resourceMovieName;
		
		ResourceMovie * m_resourceMovie;

		typedef std::map<size_t, Node *> TMapNode;
		TMapNode m_nodies;

		typedef std::map<size_t, Sprite *> TMapFlexSprite;
		TMapFlexSprite m_flexSprites;

		float m_timing;

		float m_out;

		bool m_reverse;
	};
}