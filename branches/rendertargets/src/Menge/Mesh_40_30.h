/*
 *	Mesh_40_30.h
 *
 *	Created by _Berserk_ on 21.5.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "Node.h"
#	include "Math/mat4.h"

namespace Menge
{
	class ResourceImage;

	class Mesh_40_30
		: public Node
	{
		FACTORABLE_DECLARE( Mesh_40_30 );

	public:
		Mesh_40_30();
		~Mesh_40_30();

		void loader( XmlElement* _xml ) override;

		void setAmplitude( float _amplitude );
		void setFrequency( float _frequency );
		float getAmplitude() const;
		float getFrequency() const;

	protected:
		bool _compile() override;
		void _release() override;
		void _render( Camera2D * _camera ) override;
		void _updateBoundingBox( mt::box2f& _boundingBox ) override;
		void _update( float _timing ) override;

	protected:
		size_t m_width;
		size_t m_height;
		TVertex2DVector m_vertices;
		typedef std::vector<mt::vec3f> TVec2fVector;
		TVec2fVector m_mesh;
		Material* m_material;
		String m_resourceName;
		ResourceImage* m_resourceImage;
		float m_amplitude;
		float m_freq;

		float m_timing;
	};
}	// namespace Menge
