#	pragma once

#	include "Interface/ModuleInterface.h"

#	include "CollisionGround.h"
#	include "PythonCollisionObject.h"		
#	include "CollisionMotorPosition.h"
#	include "CollisionMotorFollow.h"
#	include "PythonCollisionRadar.h"

namespace Menge
{
	class ModuleCollisionGround
		: public ModuleInterface
	{
	public:
		ModuleCollisionGround();
		~ModuleCollisionGround();

	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
		ServiceProviderInterface * getServiceProvider() const override;

	public:
		bool initialize() override;
		void finalize() override;

	public:
		void setName( const ConstString & _name ) override;
		const ConstString & getName() const override;

	public:
		CollisionGround * createCollisionGround();

	public:		
		CollisionMotorPosition * createCollisionMotorPosition( PyObject * _cb );
		CollisionMotorFollow * createCollisionMotorFollow( PyObject * _cb );		

	public:
		void update( float _time, float _timing ) override;
		void render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, unsigned int _debugMask ) override;
		
	protected:
		ServiceProviderInterface * m_serviceProvider;
		ConstString m_name;

		typedef stdex::vector<CollisionGround *> TVectorCollisionGrounds;
		TVectorCollisionGrounds m_grounds;
	};
}