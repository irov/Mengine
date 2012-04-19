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
		void setParentMovie( bool _value );
		bool isParentMovie( bool _value ) const;

		size_t getFrameCount() const;
		float getFrameDelay( size_t  _frame ) const;

	public:
		void _setReverse( bool _value) override;

	protected:
		void _setTiming( float _timing ) override;
		float _getTiming() const override;

		void _setFirstFrame() override;
		void _setLastFrame() override;

	public:
		Node * getMovieSlot( const ConstString & _name );
		float getWorkAreaDuration() const;

	protected:
		bool _play() override;
		bool _restart( size_t _enumerator ) override;
		void _stop( size_t _enumerator ) override;
		void _end( size_t _enumerator ) override;

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
		void updateFrame2D_( const MovieLayer2D & layer, Node * _node, const MovieFrame2D & _frame );

	private:
		void updateParent_();
		void playAnimation_();

	private:
		Scriptable * findInternalObject_( const ConstString & _resource, EEventName _event );
		
	protected:
		void addMovieNode_(const MovieLayer2D & _layer2D, Node * _node );
		void addMovieSlot_(const ConstString & _slotName, Node * _node );

	protected:
		void updateCurrentFrame_( size_t _lastFrame );

	protected:
		ConstString m_resourceMovieName;
		
		ResourceMovie * m_resourceMovie;

		typedef std::map<size_t, Node *> TMapNode;
		TMapNode m_nodies;

		typedef std::map<size_t, Sprite *> TMapFlexSprite;
		TMapFlexSprite m_flexSprites;

		typedef std::map<ConstString, Node *> TMapMovieSlot;
		TMapMovieSlot m_slots;

		float m_frameTiming;
		size_t m_currentFrame;

		bool m_parentMovie;
	};
}