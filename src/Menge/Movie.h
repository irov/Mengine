#	pragma once

#	include "Kernel/Node.h"
#	include "Kernel/Animatable.h"
#	include "Kernel/Soundable.h"

#   include "ResourceMovie.h"

#   include "pybind/pybind.hpp"

#   include "stdex/binary_vector.h"

namespace Menge
{
	class RenderCameraProjection;
	class RenderViewport;
	class RenderClipplane;

    class MovieNodeExtra;
    class MovieEvent;
    class MovieSceneEffect;
    class MovieInternalObject;

	class Materialable;

	class Movie;

	struct MovieLayer;
	struct MovieFrameSource;    
	//////////////////////////////////////////////////////////////////////////
	class VisitorMovieNode
	{
	public:
		virtual void visitMovieNode( Movie * _movie, Node * _node ) = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	enum MovieEventFlag
	{
		EVENT_MOVIE_GET_INTERNAL = __EVENT_ANIMATABLE_LAST__,
		EVENT_MOVIE_ACTIVATE_INTERNAL,
		EVENT_MOVIE_DEACTIVATE_INTERNAL
	};
    //////////////////////////////////////////////////////////////////////////
    class MovieEventReceiver
        : public AnimatableEventReceiver
    {
    public:
        virtual pybind::object onMovieGetInternal( const ConstString & _group, const ConstString & _name ) = 0;
        virtual Node * onMovieActivateInternal( const pybind::object & _internal ) = 0;
        virtual void onMovieDeactivateInternal( const pybind::object & _internal ) = 0;
    };
	//////////////////////////////////////////////////////////////////////////
	class Movie
		: public Node
		, public Eventable
		, public Animatable
	{
        EVENT_RECEIVER( MovieEventReceiver );

	public:
		Movie();

    public:
		void setResourceMovie( const ResourceMoviePtr & _resourceMovie );
		const ResourceMoviePtr & getResourceMovie() const;

	public:
		const ConstString & getResourceMovieName() const;
		
	public:
		void setParentMovie( bool _value );
		bool isParentMovie() const;

	public:
		bool visitMovieLayer( const ConstString & _type, VisitorMovieNode * _visitor );

	public:
		bool getMovieNode( const ConstString & _name, const ConstString & _type, Node ** _node, Movie ** _movie );
		bool hasMovieNode( const ConstString & _name, const ConstString & _type, Node ** _node, Movie ** _movie );

	public:		
		bool getMovieLayer( const ConstString & _name, const MovieLayer ** _layer, Movie ** _movie );
		bool hasMovieLayer( const ConstString & _name ) const;

	public:
		bool setEnableMovieLayer( const ConstString & _name, bool _enable );
		bool getEnableMovieLayer( const ConstString & _name, bool & _enable );

	public:
		void setEnableMovieLayers( const ConstString & _name, bool _enable );

	protected:
		void _setTiming( float _timing ) override;
		float _getTiming() const override;
				
		void _setFirstFrame() override;
		void _setLastFrame() override;

		void _setLoop( bool _value ) override;
		void _setAnimationSpeedFactor( float _factor ) override;

	protected:
		void updateLoop_( bool _value );
		void updateAnimationSpeedFactor_( float _factor );

	protected:
		bool _play( float _time ) override;
		bool _restart( uint32_t _enumerator, float _time ) override;
		void _pause( uint32_t _enumerator ) override;
		void _resume( uint32_t _enumerator, float _time ) override;
		void _stop( uint32_t _enumerator ) override;
		void _end( uint32_t _enumerator ) override;
		bool _interrupt( uint32_t _enumerator ) override;

	protected:
		bool _compile() override;
		void _release() override;
		
	protected:
		bool _activate() override;
		void _deactivate() override;

	protected:
		void _afterActivate() override;

	protected:
		void _update( float _current, float _timing ) override;

    protected:
        void _localHide( bool _value ) override;
        void _setPersonalColor( const ColourValue& _color ) override;
        void _setPersonalAlpha( float _alpha ) override;

    protected:
        void updateForward_( float _time );
        void updateBackward_();
        
        void updateForwardFrame_( float _time, uint32_t _beginFrame, uint32_t _endFrame );
        void updateForwardFrameNode_( float _time, uint32_t _beginFrame, uint32_t _endFrame, const MovieLayer &, Node * _node );

	protected:
		bool updateFrameNode_( const MovieLayer & layer, Node * _node, uint32_t _frameId, bool _interpolate, bool _start );

	protected:
		bool setupSceneEffect_();
		void enableSceneEffect_( bool _value );

		bool setupParent_();
		void removeParent_();

		void updateCamera_();

	protected:
		void stopAnimation_();
		void pauseAnimation_();
		void resumeAnimation_();

    protected:
        bool createLayers_();
		bool compileLayers_();
        void destroyLayers_();

	protected:
		bool createMovieSlot_( const MovieLayer & _layer );
        bool createMovieSceneEffect_( const MovieLayer & _layer );        
		bool createMovieNullObject_( const MovieLayer & _layer );
		bool createMovieImage_( const MovieLayer & _layer );
		bool createMovieMesh2D_( const MovieLayer & _layer );
		bool createMovieMesh2DSolid_( const MovieLayer & _layer );
		bool createMovieImageSolid_( const MovieLayer & _layer );
        bool createMovieSocketImage_( const MovieLayer & _layer );
        bool createMovieSocketShape_( const MovieLayer & _layer );
		bool createMovieAnimation_( const MovieLayer & _layer );
		bool createMovieMovie_( const MovieLayer & _layer );
        bool createMovieSubMovie_( const MovieLayer & _layer );
		bool createMovieInternalObject_( const MovieLayer & _layer );
		bool createMovieVideo_( const MovieLayer & _layer );
		bool createMovieSound_( const MovieLayer & _layer );
		bool createMovieSoundId_( const MovieLayer & _layer );
        bool createMovieText_( const MovieLayer & _layer );  
		bool createMovieTextCenter_( const MovieLayer & _layer );
		bool createMovieExtraSprite_( const MovieLayer & _layer );
        bool createMovieEvent_( const MovieLayer & _layer );
		bool createMovieParticleEmitter2_( const MovieLayer & _layer );

	protected:
		bool compileMovieText_( const MovieLayer & _layer );  

	protected:
		bool addMovieNode_( const MovieLayer & _layer, Node * _node, Animatable * _animatable, Soundable * _soundable, Movie * _movie );

	protected:
		inline Node * getLayerNode_( const MovieLayer & _layer ) const;
		inline Animatable * getLayerAnimatable_( const MovieLayer & _layer ) const;
		inline Soundable * getLayerSoundable_( const MovieLayer & _layer ) const;
		inline Movie * getLayerMovie_( const MovieLayer & _layer ) const;
		inline Node * getLayerParent_( const MovieLayer & _layer ) const;

	protected:
		void getFrameTiming_( float _time, uint32_t & _frame, float & _timing ) const;

	protected:		
		void updateTiming_();
        void updateAnimatablePlay_();
		void updateClipLastFrame_();
		void updateStartInterval_();

		void createCamera3D_();
		void destroyCamera3D_();

	protected:
		void setEnableLayer_( const MovieLayer & _layer, bool _enable );
		inline bool getEnableLayer_( const MovieLayer & _layer ) const;

		void setVisibleLayer_( const MovieLayer & _layer, bool _visible );
		inline bool getVisibleLayer_( const MovieLayer & _layer ) const;

	protected:
		bool setupBlendingMode_( const MovieLayer & _layer, Materialable * _materiable );
		
	protected:
		ResourceHolder<ResourceMovie> m_resourceMovie;

		RenderCameraProjection * m_renderCameraProjection;
		RenderViewport * m_renderViewport;

		typedef std::vector<RenderClipplane *> TVectorClipplane;
		TVectorClipplane m_clipplanes;

        struct Nodies
        {	
			Nodies()
				: node(nullptr)
				, animatable(nullptr)
				, soundable(nullptr)
				, visible(false)
				, enable(true)
				, child(false)
			{}

            Node * node;
			Animatable * animatable;
			Soundable * soundable;
			Movie * movie;

			bool visible;
			bool enable;
			bool child;
        };

		typedef stdex::vector<Nodies> TVectorNodies;
		TVectorNodies m_nodies;
				
		float m_frameTiming;
		uint32_t m_currentFrame;
        
		bool m_interruptEnd;
		bool m_parentMovie;
	};
	//////////////////////////////////////////////////////////////////////////
	inline Node * Movie::getLayerNode_( const MovieLayer & _layer ) const
	{	
		const Nodies & ns = m_nodies[_layer.index - 1];

		return ns.node;
	}
	//////////////////////////////////////////////////////////////////////////
	inline Animatable * Movie::getLayerAnimatable_( const MovieLayer & _layer ) const
	{	
		const Nodies & ns = m_nodies[_layer.index - 1];

		return ns.animatable;
	}
	//////////////////////////////////////////////////////////////////////////
	inline Soundable * Movie::getLayerSoundable_( const MovieLayer & _layer ) const
	{
		const Nodies & ns = m_nodies[_layer.index - 1];

		return ns.soundable;
	}
	//////////////////////////////////////////////////////////////////////////
	inline Movie * Movie::getLayerMovie_( const MovieLayer & _layer ) const
	{
		const Nodies & ns = m_nodies[_layer.index - 1];

		return ns.movie;
	}
	//////////////////////////////////////////////////////////////////////////
	inline Node * Movie::getLayerParent_( const MovieLayer & _layer ) const
	{
		const Nodies & ns = m_nodies[_layer.parent - 1];

		return ns.node;
	}  
	//////////////////////////////////////////////////////////////////////////
	inline bool Movie::getVisibleLayer_( const MovieLayer & _layer ) const
	{
		const Nodies & nd = m_nodies[_layer.index - 1];

		return nd.visible;
	}
	//////////////////////////////////////////////////////////////////////////
	inline bool Movie::getEnableLayer_( const MovieLayer & _layer ) const
	{
		const Nodies & nd = m_nodies[_layer.index - 1];

		return nd.enable;
	}
}