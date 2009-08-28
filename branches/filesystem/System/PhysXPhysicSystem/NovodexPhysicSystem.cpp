#	include "NovodexPhysicSystem.h"	

#	include "NovodexGeometry.h"
#	include "NovodexBallSocketJoint.h"
#	include "NovodexCapsuleController.h"
#	include <stdio.h>
#	include "Stream.h"
//////////////////////////////////////////////////////////////////////////
bool initInterfaceSystem( Menge::PhysicSystemInterface ** _ptrInterface )
{
	try
	{
		*_ptrInterface = new NovodexPhysicSystem();
	}
	catch (...)
	{
		return false;
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
void releaseInterfaceSystem( Menge::PhysicSystemInterface* _ptrInterface )
{
	delete static_cast<NovodexPhysicSystem*>(_ptrInterface);
}
//////////////////////////////////////////////////////////////////////////
NovodexPhysicSystem::NovodexPhysicSystem()
	: m_physicsSDK(0)
	, m_scene(0)
{}
//////////////////////////////////////////////////////////////////////////
NovodexPhysicSystem::~NovodexPhysicSystem()
{
	if ( m_physicsSDK )
	{
		m_controllerManager.purgeControllers();

		m_physicsSDK->releaseScene(*m_scene);
		m_physicsSDK->release();	
	}
}
//////////////////////////////////////////////////////////////////////////
float * NovodexPhysicSystem::getGravity() const
{
	static NxVec3 g(0,0,0);
	m_scene->getGravity(g);
	return g.get();
}
//////////////////////////////////////////////////////////////////////////
void NovodexPhysicSystem::init(float gx, float gy, float gz) 
{
	m_physicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION);

	assert(m_physicsSDK);

	NxSceneDesc sceneDesc;

	sceneDesc.gravity = NxVec3(gx, gy, gz);

	m_scene = m_physicsSDK->createScene(sceneDesc);

	assert(m_scene);

/*	NxSceneQueryDesc sceneQueryDesc;
	sceneQueryDesc.executeMode = NX_SQE_ASYNCHRONOUS;
	sceneQueryDesc.report = &gQueryReport;
	m_SceneQueryObject = m_scene->createSceneQuery(sceneQueryDesc);
*/

	//NxMaterial* defaultMaterial = m_scene->getMaterialFromIndex(0);
	//defaultMaterial->setRestitution(0.2f);
	//defaultMaterial->setStaticFriction(0.5f);
	//defaultMaterial->setDynamicFriction(0.5f);
}
//////////////////////////////////////////////////////////////////////////
void NovodexPhysicSystem::setGravity( float _x, float _y, float _z )
{
	m_scene->setGravity(NxVec3(_x, _y, _z));
}
//////////////////////////////////////////////////////////////////////////
void NovodexPhysicSystem::setRestitution( float _value )
{
	NxMaterial * defaultMaterial = m_scene->getMaterialFromIndex(0);
	defaultMaterial->setRestitution(_value);
}
//////////////////////////////////////////////////////////////////////////
void NovodexPhysicSystem::setStaticFriction( float _value )
{
	NxMaterial * defaultMaterial = m_scene->getMaterialFromIndex(0);
	defaultMaterial->setStaticFriction(_value);
}
//////////////////////////////////////////////////////////////////////////
void NovodexPhysicSystem::setDynamicFriction( float _value )
{
	NxMaterial * defaultMaterial = m_scene->getMaterialFromIndex(0);
	defaultMaterial->setDynamicFriction(_value);
}
//////////////////////////////////////////////////////////////////////////
void NovodexPhysicSystem::update(float _timestep)
{
	m_scene->setTiming(_timestep, 1, NX_TIMESTEP_FIXED);
	m_scene->simulate(_timestep);

	m_scene->flushStream();
	m_scene->fetchResults(NX_RIGID_BODY_FINISHED, true);

	NxReal maxTimestep;
	NxTimeStepMethod method;
	NxU32 maxIter;
	NxU32 numSubSteps;
	m_scene->getTiming(maxTimestep, maxIter, method, &numSubSteps);

	if(numSubSteps)	
	{
		m_controllerManager.updateControllers();
	}

}
//////////////////////////////////////////////////////////////////////////
void NovodexPhysicSystem::releaseCapsuleController( Menge::ControllerInterface * _capsule )
{
	delete static_cast<NovodexCapsuleController*>( _capsule );
}
//////////////////////////////////////////////////////////////////////////
Menge::ControllerInterface * NovodexPhysicSystem::createCapsuleController( float * _startPos, float _initialRadius, float _initialHeight )
{
	NxCapsuleControllerDesc desc;

	desc.interactionFlag	= NXIF_INTERACTION_INCLUDE;

	desc.position.x			= _startPos[0];
	desc.position.y			= _startPos[1];
	desc.position.z			= _startPos[2];
	desc.radius				= _initialRadius;
	desc.height				= _initialHeight;
	desc.upDirection		= NX_Y;
	desc.slopeLimit			= 0.0f;
	desc.slopeLimit			= cosf(NxMath::degToRad(45.0f));
	desc.skinWidth			= SKINWIDTH;
	desc.stepOffset			= 0;
	desc.callback			= 0;

	NxCapsuleController * contr = static_cast<NxCapsuleController*>(m_controllerManager.createController(m_scene, desc));
	Menge::ControllerInterface * cap = new NovodexCapsuleController(contr);
	return cap;
};
//////////////////////////////////////////////////////////////////////////
Menge::GeometryInterface * NovodexPhysicSystem::cookConvex( const float * _verts, int _vertexSize )
{
	NxConvexMeshDesc m_convexMesh;
	
	m_convexMesh.numVertices			= _vertexSize;
	m_convexMesh.pointStrideBytes		= 3 * sizeof(float);
	m_convexMesh.points					= _verts;
	m_convexMesh.flags					= NX_CF_COMPUTE_CONVEX | NX_CF_USE_LEGACY_COOKER;
   
	NxConvexShapeDesc * nxConvexShape = new NxConvexShapeDesc();
	
	NxInitCooking();

	MemoryWriteBuffer buf;

	bool status = NxCookConvexMesh(m_convexMesh, buf);

	assert(status);

	nxConvexShape->meshData = m_physicsSDK->createConvexMesh( MemoryReadBuffer(buf.data) );
	nxConvexShape->group = GROUP_COLLIDABLE_PUSHABLE;

	NovodexGeometry * novodexGeometry = new NovodexGeometry(nxConvexShape);
	return novodexGeometry;
}
//////////////////////////////////////////////////////////////////////////
Menge::GeometryInterface * NovodexPhysicSystem::cookConvex( const float * _verts, int _vertexSize, const int * _indecies, int _indexSize )
{
	NxConvexMeshDesc m_convexMesh;

	m_convexMesh.numVertices			= _vertexSize;
	m_convexMesh.pointStrideBytes		= 3 * sizeof(float);
	m_convexMesh.points					= _verts;
	m_convexMesh.numTriangles			= _indexSize;
	m_convexMesh.triangles				= _indecies;
	m_convexMesh.triangleStrideBytes	= 3 * sizeof(int);
	m_convexMesh.flags					= 0;

	NxConvexShapeDesc * nxConvexShape = new NxConvexShapeDesc();

	NxInitCooking();
	MemoryWriteBuffer buf;

	bool status = NxCookConvexMesh(m_convexMesh, buf);
	assert(status);

	nxConvexShape->meshData = m_physicsSDK->createConvexMesh(MemoryReadBuffer(buf.data));
	nxConvexShape->group = GROUP_COLLIDABLE_PUSHABLE;

	NovodexGeometry * novodexGeometry = new NovodexGeometry(nxConvexShape);
	return novodexGeometry;
}
//////////////////////////////////////////////////////////////////////////
Menge::GeometryInterface * NovodexPhysicSystem::cookConcave( const float * _verts, int _vertexSize, const int * _indecies, int _indexSize )
{
	NxTriangleMeshDesc triMeshDesc;

	triMeshDesc.numVertices			= _vertexSize;
	triMeshDesc.pointStrideBytes	= 3 * sizeof(float);
	triMeshDesc.points				= _verts;
	triMeshDesc.numTriangles		= _indexSize;
	triMeshDesc.triangles			= _indecies;
	triMeshDesc.triangleStrideBytes	= 3 * sizeof(int);
	triMeshDesc.flags				= 0;

	NxTriangleMeshShapeDesc * nxTriShape = new NxTriangleMeshShapeDesc();

	NxInitCooking();
	MemoryWriteBuffer buf;

	//bool status = NxCookTriangleMesh(triMeshDesc, UserStream("c:\\cooked.bin", false));

	bool status = NxCookTriangleMesh(triMeshDesc, buf);
	assert(status);

	nxTriShape->meshData = m_physicsSDK->createTriangleMesh(MemoryReadBuffer(buf.data));
	nxTriShape->group = GROUP_COLLIDABLE_NON_PUSHABLE;

	NovodexGeometry * novodexGeometry = new NovodexGeometry(nxTriShape);
	return novodexGeometry;
}
//////////////////////////////////////////////////////////////////////////
Menge::GeometryInterface * NovodexPhysicSystem::cookConvex( const char * _filename )
{
	NxConvexShapeDesc * nxTriShape = new NxConvexShapeDesc();

	nxTriShape->meshData = m_physicsSDK->createConvexMesh(UserStream(_filename, true));
	nxTriShape->group = GROUP_COLLIDABLE_PUSHABLE;

	NovodexGeometry * novodexGeometry = new NovodexGeometry(nxTriShape);

	return novodexGeometry;
}
//////////////////////////////////////////////////////////////////////////
Menge::GeometryInterface * NovodexPhysicSystem::cookConcave( const char * _filename )
{
	NxTriangleMeshShapeDesc * nxTriShape = new NxTriangleMeshShapeDesc();

	nxTriShape->meshData = m_physicsSDK->createTriangleMesh(UserStream(_filename, true));
	nxTriShape->group = GROUP_COLLIDABLE_PUSHABLE;

	NovodexGeometry * novodexGeometry = new NovodexGeometry(nxTriShape);

	return novodexGeometry;
}
//////////////////////////////////////////////////////////////////////////
Menge::GeometryInterface * NovodexPhysicSystem::cookBox( float _width, float _height, float _depth )
{
	NxBoxShapeDesc * nxBoxDesc = new NxBoxShapeDesc();
	nxBoxDesc->dimensions = NxVec3( _width * 0.5f, _height * 0.5f, _depth * 0.5f );
	nxBoxDesc->group = GROUP_COLLIDABLE_PUSHABLE;

	NovodexGeometry * novodexGeometry = new NovodexGeometry(nxBoxDesc);
	return novodexGeometry;
}
//////////////////////////////////////////////////////////////////////////
Menge::RigidBodyInterface * NovodexPhysicSystem::createRigidBody( float _density, bool _dynamic, const Menge::GeometryInterface * _geometry)
{
	NxActorDesc actorDesc;
	NxBodyDesc	bodyDesc;
		
	const NovodexGeometry * novodexGeom = static_cast<const NovodexGeometry*>(_geometry);
	NxShapeDesc * shapeDesc = novodexGeom->nxShape;

	actorDesc.shapes.pushBack(shapeDesc);

	actorDesc.density = 0; 
	actorDesc.body = 0;

	//actorDesc.body = 0;

	if (_dynamic == true) 
	{
		actorDesc.density = _density; 
		//bodyDesc.mass = 1;
		//bodyDesc.massSpaceInertia = NxVec3( 2.6666667, 2.6666667, 2.6666667 );
		actorDesc.body = &bodyDesc;
	} 
	else
	{
		//bodyDesc.mass = 0;
		actorDesc.density = 0; 
		actorDesc.body = 0;
	}

	NxActor * _actor = m_scene->createActor(actorDesc);
	
	Menge::RigidBodyInterface * rigidBody = new NovodexBody(_actor);

	return rigidBody;
}
//////////////////////////////////////////////////////////////////////////
void	NovodexPhysicSystem::removeRigidBody( Menge::RigidBodyInterface * _rigidBody )
{
	NovodexBody * novodexBody = static_cast<NovodexBody*>(_rigidBody);

	NxActor * actor = novodexBody->getNxActor();

	if (actor) 
	{
		m_scene->releaseActor(*actor);
	}

	delete novodexBody;
}
//////////////////////////////////////////////////////////////////////////
void	NovodexPhysicSystem::createJoint(Menge::RigidBodyInterface * body0, Menge::RigidBodyInterface * body1, float x, float y, float z)
{
	Menge::BallSocketJointInterface * joint = new NovodexBallSocketJoint(m_scene);
	joint->init(body0,body1,x,y,z);
	joint->setLimits(3.14f,3.14f);
}
//////////////////////////////////////////////////////////////////////////
void NovodexPhysicSystem::removeGeometry( Menge::GeometryInterface * _geom )
{
	delete _geom;
}
//////////////////////////////////////////////////////////////////////////