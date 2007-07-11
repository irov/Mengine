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
		float		emission;	// кол частиц
		mt::vec2f	life;		//продолж их жизни min/max
		float		dir;		//направление в котором будут двигаццо частицы, от 0 до 360
		float		spread;		//рассеи€ние частиц вдоль направлений, от 0 до 360
		mt::vec2f	v;			//скорость частиц min/max
		mt::vec2f	g;			//гравицта€ частий min/max
		mt::vec2f	r;			//нормальное ускорение
		mt::vec2f	tan;		//тангециальное ускорение, 
		mt::vec3f	size;		//размер частиц, min, max, var
		mt::vec3f	spin;		//вращение,  min, max, var
		mt::vec4f	startColor; //начальный цвет
		mt::vec4f	endColor;	//конечный цвет. т.е. когда у нас частица начинает пропадать, то она мен€ет на конечный цвет
		mt::vec2f	varColor;	//x - как измен€цо цвет, y - как измен€ецо альфа канал
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