/*
 *	Vertex.h
 *
 *	Created by _Berserk_ on 19.5.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "Config/Typedef.h"

#define VDECL_XYZ		0x002
#define VDECL_NORMAL	0x010
#define VDECL_DIFFUSE	0x040
#define VDECL_SPECULAR	0x080
#define VDECL_TEX1		0x100
#define VDECL_TEX2		0x200

namespace Menge
{
	//struct Vertex
	//{
	//	static const uint32 declaration;
	//};

	struct Vertex2D
	//	: public Vertex
	{
		float pos[3];
		uint32 color;
		float uv[2];

		static const uint32 declaration;
	};

	struct Vertex3D
	//	: public Vertex
	{
		float pos[3];
		float n[3];
		uint32 color;
		float uv[2];

		static const uint32 declaration;
	};

	typedef std::vector<uint16> TVectorIndices;
	typedef std::vector<Vertex2D> TVectorVertex2D;
	typedef std::vector<Vertex3D> TVectorVertex3D;

	class ApplyColor2D
	{
	public:
		ApplyColor2D( uint32 _argb )
			: m_argb( _argb )
		{
		}
		void operator()( Vertex2D& _vtx )
		{
			_vtx.color = m_argb;
		}

	protected:
		uint32 m_argb;
	};

	class ApplyZ
	{
	public:
		ApplyZ( float _z )
			: m_z( _z )
		{
		}
		void operator()( Vertex2D& _vtx )
		{
			_vtx.pos[2] = m_z;
		}
	protected:
		float m_z;
	};

}	// namespace Menge
