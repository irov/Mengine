#	pragma once

#	include "Config/Typedef.h"


#	include "math\vec2.h"
#	include "math\polygon.h"
#	include "math\box2.h"

#	include "RigidBody2D.h"

namespace Menge
{
	class RenderImageInterface;

	class ResourceImage;

	class Tile
	{
	public:
		Tile(float _minAngle, float _maxAngle, const String & _imagename);
		~Tile();

	public:
		void create(const mt::TVectorPoints & _poly, const mt::vec2f & _juncSize);
		void release();
		void renderTile(const mt::mat3f & _worldMatrix, const RenderImageInterface * _imageJunc);
		void updateBoundingBox(mt::box2f & _boundingBox);
		void invalidateTile();

#	ifndef MENGE_MASTER_RELEASE
		void renderDebug(const mt::mat3f & _worldMatrix, unsigned long _color);
#	endif

	private:
		RigidBody2D * m_rigidBodySensor;

		float m_minAngle;
		float m_maxAngle;
		String m_imagename;
		const RenderImageInterface * m_image;
		ResourceImage * m_imageResource;

		struct Quad
		{
			mt::TVectorPoints v; //FIXME: !!
			float s;
			float t;
		};

		std::vector<Quad> m_geometry;
		std::vector<Quad> m_junks;

		bool m_invalidateRenderTile;
	};
};