#	include "FFCamera.h"
#	include "Interface/RenderSystemInterface.h"
#	include "RenderEngine.h"
#	include "Holder.h"
#	include "Actor.h"
#	include "DiscreteEntity.h"

namespace Menge
{
	FFCamera3D::FFCamera3D()
	: m_cameraPos(0,20,0)
	, m_cameraForward(0,0,-1)
	, m_cameraRight(1,0,0)
	, m_fMinYawAngle(-3.1415f/2.f)
	, m_fMaxYawAngle(3.1415f/2.f)
	, m_fRotateSpeed(0.01f)
	, m_actor(0)
	{
	}

	FFCamera3D::~FFCamera3D()
	{}

	void FFCamera3D::setActor(Actor * _actor)
	{
		m_actor = _actor;
	}

	void FFCamera3D::update(float _timing)
	{
		if(m_actor)
		{
			DiscreteEntity * ent = m_actor->getChildrenT<DiscreteEntity>("BarrelNode",true);
			if(m_interface->isAABBIntersect(ent->get()) == 0)
			{
				m_interface->translate(mt::vec3f(1,0,0).m);
			}

		//	float min[3];
		//	float max[3];
		//	m_interface->getAABB(min,max);
		//	printf("min x = %f, y = %f, z = %f \n",min[0],min[1],min[2]);
		}
	//	rotate(_timing);
		m_interface->lookAt(
			m_cameraPos.x + m_cameraForward.x,
			m_cameraPos.y + m_cameraForward.y, 
			m_cameraPos.z + m_cameraForward.z);
	}

	void FFCamera3D::forward( float s )
	{
		m_cameraPos += m_cameraForward * s;
		m_interface->setPosition(m_cameraPos.x, m_cameraPos.y, m_cameraPos.z);
	}

	void FFCamera3D::left( float s )
	{
		m_cameraPos += m_cameraRight * s;
		m_interface->setPosition(m_cameraPos.x, m_cameraPos.y, m_cameraPos.z);
	}

	void FFCamera3D::rotate( float t )
	{
		// Вращение камеры на персонажа

		mt::vec3f cameraDir = *(mt::vec3f*)m_interface->getDirection();

		mt::quatf orientDir = *(mt::quatf*)m_interface->getOrient();
		cameraDir = orientDir * mt::vec3f(0,0,-1);

		float curAngle = mt::get_axis_angle(cameraDir,1);

		printf("curAngle = %f \n", curAngle);

		mt::vec3f charPos(0,0,0);

		mt::vec3f dir = charPos - *(mt::vec3f*)m_interface->getPosition();
		
		mt::norm_v3(dir,dir);
		float toCharacterAngle	= mt::get_axis_angle(dir,1);
		
	//	if(toCharacterAngle > m_fMaxYawAngle)	toCharacterAngle	= m_fMaxYawAngle;
	//	if(toCharacterAngle < m_fMinYawAngle)	toCharacterAngle	= m_fMinYawAngle;

		float angle = toCharacterAngle - curAngle;

		printf("toCharacterAngle = %f \n", angle);

		if(fabs(angle) > 0.00001)
		{
			printf("%f \n",t);
			// Делаем вращение камерой плавное
			if(fabs(angle) > t * m_fRotateSpeed)
			{
				// Сильно быстро двигается камера, замедляем скорость
			/*	if(angle > 0)
					angle	= t * m_fRotateSpeed;
				else
					angle	= -(t * m_fRotateSpeed);*/
			}

			mt::quatf q = mt::q_from_angle_axis(mt::vec3f(0, -1, 0),angle * 180.f / 3.141596f);

			//m_interface->setOrient(q.m);
		//	m_interface->rotate(mt::vec3f(0, -1, 0).m, angle* 180.f / 3.141596f);
			m_interface->setDirection(dir.x,dir.y,dir.z);
//			m_Cameraf.Rotate(vec3f(0, 0, -1), (angle) * 180.f / 3.141596f);
		}
	}

	void FFCamera3D::set2Direction(int x, int y)
	{
		static int mx = 0;
		static int my = 0;

		int dx = mx - x;
		int dy = my - y;

		mt::norm_v3(m_cameraForward);
		m_cameraRight.cross(m_cameraForward,mt::vec3f(0,1,0));

		mt::quatf qx(3.1415f * dx * 20.0f / 180.0f,mt::vec3f(0,1,0));
		qx.rotate(m_cameraForward);

		mt::quatf qy(3.1415f * dy * 20.0f / 180.0f,m_cameraRight);
		qy.rotate(m_cameraForward);

		mx = x;
		my = y;
	}

	bool FFCamera3D::activate()
	{
		std::string m_name = "FreeCamera";

		m_interface = Holder<RenderEngine>::hostage()->createCamera(
			m_name );

		m_interface->setAspectRatio( 1.3f );
		m_interface->setNearClipDistance( 10.0f );
		m_interface->setFarClipDistance( 1000.0f );
		m_interface->setPosition( 0, 70, 0 );

		m_interface->lookAt(0,0,0);

		return true;
	}
}