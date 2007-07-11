#	pragma once

#	include "ResourceImpl.h"

#	include "Interface/RenderSystemInterface.h"

#	include <vector>

#	include "math/vec2.h"
#	include "math/vec4.h"

namespace Menge
{
	struct ParticleDesc
	{
		float		emission;	// ��� ������
		mt::vec2f	life;		//������� �� ����� min/max
		float		dir;		//����������� � ������� ����� �������� �������, �� 0 �� 360
		float		spread;		//���������� ������ ����� �����������, �� 0 �� 360
		mt::vec2f	v;			//�������� ������ min/max
		mt::vec2f	g;			//��������� ������ min/max
		mt::vec2f	r;			//���������� ���������
		mt::vec2f	tan;		//������������� ���������, 
		mt::vec3f	size;		//������ ������, min, max, var
		mt::vec3f	spin;		//��������,  min, max, var
		mt::vec4f	startColor; //��������� ����
		mt::vec4f	endColor;	//�������� ����. �.�. ����� � ��� ������� �������� ���������, �� ��� ������ �� �������� ����
		mt::vec2f	varColor;	//x - ��� �������� ����, y - ��� ��������� ����� �����
	};

/*
	class ResourceParticleEmitter
		: public ResourceImpl
	{
	public:
		ResourceParticleEmitter( const std::string & _name );
	public:
		RenderImageInterface * getImage() = 0;
	private:
		std::string			m_filename;
		RenderImageInterface*	m_image; 
	};
*/
	class ResourceParticle
		: public ResourceImpl
	{
		RESOURCE_DECLARE( ResourceParticle )

	public:
		ResourceParticle( const std::string & _name );

	public:
		void loader( TiXmlElement *xml );

	protected:
		bool _compile();
		void _release();

	public:
	//	const ParticleDesc&	getDesc(); 
		RenderImageInterface * getImage();

	private:
		std::string			m_filename;
		ParticleDesc		m_desc;
		RenderImageInterface*	m_image; 
	};
}