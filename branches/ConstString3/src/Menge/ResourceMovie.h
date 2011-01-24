#	pragma once

#	include "ResourceReference.h"

#	include "Math/vec2.h"

#	include <vector>

namespace Menge
{
	struct MovieLayer
	{
		int index;
		String name;
		float width;
		float height;

		float in;
		float out;

		String path;

		struct KeyAnchorPoint
		{
			mt::vec2f value;
			float time;
			int interp;
		};

		typedef std::vector<KeyAnchorPoint> TVectorKeyAnchorPoints;
		TVectorKeyAnchorPoints anchorPoints;

		struct KeyPosition
		{
			mt::vec2f value;
			float time;
			int interp;
		};

		typedef std::vector<KeyPosition> TVectorKeyPositions;
		TVectorKeyPositions positions;

		struct KeyScale
		{
			mt::vec2f value;
			float time;
			int interp;
		};

		typedef std::vector<KeyScale> TVectorKeyScales;
		TVectorKeyScales scales;

		struct KeyRotation
		{
			float value;
			float time;
			int interp;
		};

		typedef std::vector<KeyRotation> TVectorKeyRotations;
		TVectorKeyRotations rotations;

		struct KeyOpacity
		{
			float value;
			float time;
			int interp;
		};

		typedef std::vector<KeyOpacity> TVectorKeyOpacities;
		TVectorKeyOpacities opacities;
	};

	class ResourceMovie
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceMovie )

	public:
		ResourceMovie();
		~ResourceMovie();

	public:
		void setPathMOV( const ConstString & _path );
		const ConstString & getPathMOV() const;

	public:
		std::size_t getLayerSize() const;
		MovieLayer & getLayer( std::size_t _index );

	protected:
		void loader( BinParser * _parser ) override;

	protected:
		bool _compile() override;
		void _release() override;

	protected:
		bool readMOV_( const ConstString & _path );

	protected:
		ConstString m_pathMOV;

		String m_name;

		float m_duration;
		float m_width;
		float m_height;

		typedef std::vector<MovieLayer> TVectorMovieLayers;
		TVectorMovieLayers m_layers;
	};
}