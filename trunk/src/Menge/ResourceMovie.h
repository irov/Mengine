#	pragma once

#	include "Interface/MovieKeyFrameInterface.h"

#	include "Kernel/ResourceReference.h"

#	include "Math/vec2.h"
#	include "Math/vec3.h"
#	include "Math/mat4.h"

#   include "Core/Polygon.h"

#	include <stdex/stl_vector.h>

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
	const size_t MOVIE_LAYER_UNSTOPPABLE = 0x0200;
	
    const size_t movie_layer_parent_none = (uint32_t)-1;

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

		uint32_t id;
		uint32_t index;
		uint32_t parent;

        uint32_t playCount;        
        float scretch;

		float in;
		float out;
		float startInterval;

        uint32_t state;

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

		bool isUnstoppable() const
		{
			return (state & MOVIE_LAYER_UNSTOPPABLE) > 0;
		}
	};

	typedef stdex::vector<MovieLayer> TVectorMovieLayers;

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

		uint32_t getFrameCount() const;
		uint32_t getMaxLayerIndex() const;

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
		DataInterfacePtr compileData_( const FilePath & _path );

	protected:
		bool isThreeDNode( size_t _index ) const;

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

		uint32_t m_frameCount;

		mt::vec2f m_size;

		TVectorMovieLayers m_layers;

		bool m_hasCamera3D;
		MovieLayerCamera3D m_camera3D;

		uint32_t m_maxLayerIndex;
	};
	//////////////////////////////////////////////////////////////////////////
	inline const MovieFramePackInterfacePtr & ResourceMovie::getFramePack() const
	{
		return m_keyFramePack;
	}	
}