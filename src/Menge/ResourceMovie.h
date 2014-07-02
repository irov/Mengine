#	pragma once

#	include "Interface/MovieKeyFrameInterface.h"

#	include "Kernel/ResourceReference.h"

#	include "Math/vec2.h"
#	include "Math/vec3.h"
#	include "Math/mat4.h"

#   include "Core/Polygon.h"

#	include <vector>
#	include <map>


namespace Menge
{
	class ResourceShape;

    const size_t MOVIE_LAYER_NODE = 0x0001;
    const size_t MOVIE_LAYER_ANIMATABLE = 0x0002;
    const size_t MOVIE_LAYER_MOVIE = 0x0004;
    const size_t MOVIE_LAYER_THREED = 0x0008;
    const size_t MOVIE_LAYER_AUDIO = 0x0010;
    const size_t MOVIE_LAYER_EVENT = 0x0020;
    const size_t MOVIE_LAYER_EXTRA = 0x0040;
    const size_t MOVIE_LAYER_SUB_MOVIE = 0x0080;
    const size_t MOVIE_LAYER_MESH_2D = 0x0100;
	
    const size_t movie_layer_parent_none = (size_t)-1;

	struct MovieLayer
	{
		MovieLayer()
			: id(0)
			, index(movie_layer_parent_none)
			, parent(0)
            , timeRemap(false)
			, shape(false)
			, polygon(false)
            , playCount(1)
            , scretch(1.f)
            , loop(false)
			, in(0.f)
			, out(0.f)
			, startInterval(0.f)
            , state(0)
			, immutable(false)
			, switcher(false)
		{
		}

		ConstString name;
		ConstString type;

		size_t id;
		size_t index;
		size_t parent;

        size_t playCount;        
        float scretch;

		float in;
		float out;
		float startInterval;

        size_t state;

		ConstString blendingMode;
		ConstString keyFramesName;
		FilePath keyFramesPath;

		ConstString source;		

		bool immutable;
		bool timeRemap;
		bool shape;
		bool polygon;
		bool loop;
		bool switcher;

        bool isNode() const
        {
            return (state & MOVIE_LAYER_NODE) > 0;
        }
        
		bool isAnimatable() const
        {
            return (state & MOVIE_LAYER_ANIMATABLE) > 0;
        }

		bool isMovie() const
        {
            return (state & MOVIE_LAYER_MOVIE) > 0;
        }

        bool isMesh2D() const
        {
            return (state & MOVIE_LAYER_MESH_2D) > 0;
        }

		bool isThreeD() const
        {
            return (state & MOVIE_LAYER_THREED) > 0;
        }

		bool isAudio() const
        {
            return (state & MOVIE_LAYER_AUDIO) > 0;
        }

        bool isEvent() const
        {
            return (state & MOVIE_LAYER_EVENT) > 0;
        }

        bool isExtra() const
        {
            return (state & MOVIE_LAYER_EXTRA) > 0;
        }

        bool isSubMovie() const
        {
            return (state & MOVIE_LAYER_SUB_MOVIE) > 0;
        }
	};

	typedef std::vector<MovieLayer> TVectorMovieLayers;

	struct MovieLayerCamera3D
	{
		float width;
		float height;

		mt::vec3f cameraPosition;
		mt::vec3f cameraInterest;
		float cameraFOV;
		float cameraAspect;
	};

	struct MovieLayer3D
		: public MovieLayer
	{
		MovieLayerCamera3D camera;
	};

	struct MovieFootage
	{
		ConstString path;
	};

	struct MovieInternal
	{
		ConstString group;
	};

	class VisitorResourceMovie
	{
	public:
		virtual void visitLayer( const MovieFramePackInterfacePtr & _framePack, const MovieLayer & _layer ) = 0;
	};

	class ResourceMovie
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceMovie )

	public:
		ResourceMovie();
		~ResourceMovie();

	public:
		float getFrameDuration() const;
		float getDuration() const;

		size_t getFrameCount() const;
		size_t getMaxLayerIndex() const;

		const mt::vec2f & getSize() const;

        const mt::vec2f & getLoopSegment() const;

	public:
		const FilePath & getFileName() const;
		const ConstString & getDataflowType() const;

    public:
		const TVectorMovieLayers & getLayers() const;
		const MovieLayerCamera3D & getCamera3D() const;

	public:
		ResourceShape * getSocketShape( const ConstString & _socketName ) const;

	public:
		bool hasAnchorPoint() const;
		const mt::vec3f & getAnchorPoint() const;

	public:
		bool hasCamera3D() const;

	public:
		inline const MovieFramePackInterfacePtr & getFramePack() const;

	public:
		bool hasLayer( const ConstString & _name, const MovieLayer ** _layer ) const;

    public:
		void visitResourceMovie( VisitorResourceMovie * _visitor );

    protected:
        bool _isValid() const override;

	protected:
		bool _loader( const Metabuf::Metadata * _parser ) override;
		bool _convert() override;

	protected:
		bool _compile() override;
		void _release() override;

	protected:
		bool isThreeDNode( size_t _index );

	protected:		
		float m_frameDuration;
		float m_duration;

        mt::vec2f m_loopSegment;

		bool m_hasAnchorPoint;
		mt::vec3f m_anchorPoint;
		
        FilePath m_path;
		ConstString m_dataflowType;
		ConstString m_converterType;

		MovieFramePackInterfacePtr m_keyFramePack;

		size_t m_frameCount;

		mt::vec2f m_size;

		TVectorMovieLayers m_layers;

		bool m_hasCamera3D;
		MovieLayerCamera3D m_camera3D;

		size_t m_maxLayerIndex;
	};
	//////////////////////////////////////////////////////////////////////////
	inline const MovieFramePackInterfacePtr & ResourceMovie::getFramePack() const
	{
		return m_keyFramePack;
	}	
}