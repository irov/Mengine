#	pragma once

#	include "Node.h"
#	include "Animatable.h"

namespace Menge
{
	class Camera3D;

	struct MovieLayer;
	struct MovieFrameSource;

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
		Node * getMovieSlot( const ConstString & _name ) const;
		bool hasMovieSlot( const ConstString & _name ) const;

	protected:
		void _setReverse( bool _reverse ) override;

	protected:
		void updateReverse_( bool _reverse );

	protected:
		void _setTiming( float _timing ) override;
		float _getTiming() const override;

		void _setFirstFrame() override;
		void _setLastFrame() override;

		void _setSpeedFactor( float _factor ) override;

	protected:
		bool _play() override;
		bool _restart( size_t _enumerator ) override;
		void _stop( size_t _enumerator ) override;
		void _end( size_t _enumerator ) override;
		void _interrupt( size_t _enumerator ) override;

	protected:
		bool _compile() override;
		void _release() override;
		
	protected:
		bool _activate() override;
		void _deactivate() override;

	protected:
		void _update( float _timing ) override;

	protected:
		void _setEventListener( PyObject * _embed ) override;

	private:
		void updateFrameNode_( const MovieLayer & layer, Node * _node, const MovieFrameSource & _frame );

	private:
		void updateParent_();
		void updateCamera_();
		void stopAnimation_();

	private:
		PyObject * findInternalObject_( const ConstString & _resource ) const;

	protected:
		bool createMovieSlot_( const MovieLayer & _layer );
		bool createMovieNullObject_( const MovieLayer & _layer );
		bool createMovieImage_( const MovieLayer & _layer );
		bool createMovieImageSolid_( const MovieLayer & _layer );
		bool createMovieAnimation_( const MovieLayer & _layer );
		bool createMovieMovie_( const MovieLayer & _layer );
		bool createMovieInternalObject_( const MovieLayer & _layer );
		bool createMovieVideo_( const MovieLayer & _layer );
		bool createMovieSound_( const MovieLayer & _layer );
		bool createMovieEmitterContainer_( const MovieLayer & _layer );


	protected:
		void addMovieNode_( const MovieLayer & _layer, Node * _node );

	protected:
		void updateCurrentFrame_( size_t _lastFrame, bool _force );
		void updateTiming_();
		void updateClipLastFrame_();
		void createCamera3D_();

	protected:
		ConstString m_resourceMovieName;
		
		ResourceMovie * m_resourceMovie;

		Camera3D * m_renderCamera3D;

		typedef std::vector<Node *> TVectorNodies;
		TVectorNodies m_nodies;
		
		typedef std::map<ConstString, Node *> TMapMovieSlot;
		TMapMovieSlot m_slots;

		float m_frameTiming;
		size_t m_currentFrame;

		bool m_parentMovie;
	};
}