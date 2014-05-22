#	pragma once

#	include "Kernel/Node.h"
#	include "Kernel/Animatable.h"

#   include "ResourceMovie.h"

#   include "stdex/binary_vector.h"

namespace Menge
{
	class Camera3D;
	class RenderViewport;

    class MovieNodeExtra;
    class MovieEvent;
    class MovieSceneEffect;
    class MovieInternalObject;

    class HotSpot;

	struct MovieLayer;
	struct MovieFrameSource;    

    class VisitorMovieSocket
    {
    public:
        virtual void visitSocket( const ConstString & _name, HotSpot * _hotspot ) = 0;
    };

	class Movie
		: public Node
		, public Animatable
	{
	public:
		Movie();

    public:
        void setResourceMovie( ResourceMovie * _resourceMovie );
        ResourceMovie * getResourceMovie() const;
		
	public:
		void setParentMovie( bool _value );
		bool isParentMovie() const;

	public:
		Node * getMovieSlot( const ConstString & _name ) const;
		bool hasMovieSlot( const ConstString & _name ) const;

    public:
        Movie * getSubMovie( const ConstString & _name ) const;
        bool hasSubMovie( const ConstString & _name ) const;

    public:
        bool visitSockets( VisitorMovieSocket * _visitor ) const;

        HotSpot * getSocket( const ConstString & _name ) const;
        bool hasSocket( const ConstString & _name ) const;

    public:
        bool setMovieEvent( const ConstString & _name, PyObject * _cb ) const;
        bool hasMovieEvent( const ConstString & _name ) const;

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
		bool _play( float _time ) override;
		bool _restart( float _time, size_t _enumerator ) override;
		void _stop( size_t _enumerator ) override;
		void _end( size_t _enumerator ) override;
		bool _interrupt( size_t _enumerator ) override;

	protected:
		bool _compile() override;
		void _release() override;
		
	protected:
		bool _activate() override;
		void _deactivate() override;

	protected:
		void _update( float _current, float _timing ) override;

    protected:
        void _localHide( bool _value ) override;
        void _setPersonalColor( const ColourValue& _color ) override;
        void _setPersonalAlpha( float _alpha ) override;

    protected:
        void updateForward_( float _time );
        void updateBackward_();
        
        void updateForwardFrame_( float _time, size_t _beginFrame, size_t _endFrame );
        void updateBackwardFrame_( size_t _beginFrame, size_t _endFrame );

        void updateForwardFrameNode_( float _time, size_t _beginFrame, size_t _endFrame, const MovieLayer &, Node * _node );

	protected:
		void _setEventListener( PyObject * _embed ) override;

	private:
		bool updateFrameNode_( const MovieLayer & layer, Node * _node, size_t _frameId, bool _interpolate );

	private:
		bool setupParent_();
		void removeParent_();

		void updateCamera_();
		void stopAnimation_();

    protected:
        bool createLayers_();
        void releaseLayers_();

	protected:
		bool createMovieSlot_( const MovieLayer & _layer );
        bool createMovieSceneEffect_( const MovieLayer & _layer );        
		bool createMovieNullObject_( const MovieLayer & _layer );
		bool createMovieImage_( const MovieLayer & _layer );
		bool createMovieMesh2D_( const MovieLayer & _layer );
		bool createMovieImageSolid_( const MovieLayer & _layer );
        bool createMovieSocketImage_( const MovieLayer & _layer );
        bool createMovieSocketShape_( const MovieLayer & _layer );
		bool createMovieAnimation_( const MovieLayer & _layer );
		bool createMovieMovie_( const MovieLayer & _layer );
        bool createMovieSubMovie_( const MovieLayer & _layer );
		bool createMovieInternalObject_( const MovieLayer & _layer );
		bool createMovieVideo_( const MovieLayer & _layer );
		bool createMovieSound_( const MovieLayer & _layer );
        bool createMovieText_( const MovieLayer & _layer );  
		bool createMovieTextCenter_( const MovieLayer & _layer );
        bool createMovieEvent_( const MovieLayer & _layer );
		bool createMovieParticleEmitter_( const MovieLayer & _layer );
		bool createMovieParticleEmitter2_( const MovieLayer & _layer );


	protected:
		void addMovieNode_( const MovieLayer & _layer, Node * _node );
		Node * getMovieNode_( const MovieLayer & _layer ) const;
		Node * getMovieParent_( const MovieLayer & _layer ) const;

	protected:		
		void updateTiming_();
        void updateAnimatablePlay_();
		void updateClipLastFrame_();
		void updateStartInterval_();

		void createCamera3D_();
		void destroyCamera3D_();
		
	protected:
		ResourceHolder<ResourceMovie> m_resourceMovie;

		Camera3D * m_renderCamera3D;
		RenderViewport * m_renderViewport;

        struct Nodies
        {
            bool child;
            Node * node;
        };

		typedef std::vector<Nodies> TVectorNodies;
		TVectorNodies m_nodies;
		
		typedef stdex::binary_vector<ConstString, Node *> TMapMovieSlots;
		TMapMovieSlots m_slots;

        typedef stdex::binary_vector<ConstString, Movie *> TMapSubMovies;
        TMapSubMovies m_subMovies;

        typedef stdex::binary_vector<ConstString, HotSpot *> TMapSockets;
        TMapSockets m_sockets;

        typedef stdex::binary_vector<ConstString, MovieEvent *> TMapMovieEvent;
        TMapMovieEvent m_events;

        typedef stdex::binary_vector<ConstString, MovieSceneEffect *> TMapSceneEffects;
        TMapSceneEffects m_sceneEffects;

        typedef stdex::binary_vector<ConstString, MovieInternalObject *> TMapInternalObjects;
        TMapInternalObjects m_internals;

		float m_frameTiming;
		size_t m_currentFrame;
        
		bool m_parentMovie;
	};
}