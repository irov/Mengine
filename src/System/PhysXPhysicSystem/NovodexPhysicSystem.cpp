#	include "NovodexPhysicSystem.h"	

#	include "NovodexGeometry.h"
#	include "NovodexBallSocketJoint.h"
#	include <stdio.h>
#	include "Stream.h"
//////////////////////////////////////////////////////////////////////////
bool initInterfaceSystem( PhysicSystemInterface ** _ptrInterface )
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
void releaseInterfaceSystem( PhysicSystemInterface* _ptrInterface )
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
		m_physicsSDK->releaseScene(*m_scene);
		m_physicsSDK->release();	
	}
}
static NxRaycastHit     gHits[100];
int numHits = 0;

class myRaycastQueryReport : public NxSceneQueryReport
{
	virtual	NxQueryReportResult	onBooleanQuery(void* userData, bool result){ return NX_SQR_CONTINUE; };
	virtual	NxQueryReportResult	onShapeQuery(void* userData, NxU32 nbHits, NxShape** hits){ return NX_SQR_CONTINUE; };
	virtual	NxQueryReportResult	onSweepQuery(void* userData, NxU32 nbHits, NxSweepQueryHit* hits){ return NX_SQR_CONTINUE; };

	virtual	NxQueryReportResult	onRaycastQuery(void* userData, NxU32 nbHits, const NxRaycastHit* hits)
	{
		//unsigned int i = (unsigned int)userData;

		printf("%d \n", nbHits);
		
		
		if(nbHits > 0)
		{
		printf("%f \n",hits[0].distance);
		gHits[0] = hits[0];
		}


	/*	if (nbHits > 0) 
		{
			gHits[i] = hits[0];
		} 
		else 
		{
			gHits[i].shape = NULL;
		}*/
		return NX_SQR_CONTINUE;
	}
}gQueryReport;
//////////////////////////////////////////////////////////////////////////
void NovodexPhysicSystem::init(float gx, float gy, float gz) 
{
	m_physicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION);

	assert(m_physicsSDK);

	NxSceneDesc sceneDesc;

	sceneDesc.gravity = NxVec3(gx, gy, gz);

	m_scene = m_physicsSDK->createScene(sceneDesc);

	assert(m_scene);

	NxSceneQueryDesc sceneQueryDesc;
	sceneQueryDesc.executeMode = NX_SQE_ASYNCHRONOUS;
	sceneQueryDesc.report = &gQueryReport;
	m_SceneQueryObject = m_scene->createSceneQuery(sceneQueryDesc);


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
}
//////////////////////////////////////////////////////////////////////////
GeometryInterface * NovodexPhysicSystem::cookConvex( const float * _verts, int _vertexSize )
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

	NovodexGeometry * novodexGeometry = new NovodexGeometry(nxConvexShape);
	return novodexGeometry;
}
//////////////////////////////////////////////////////////////////////////
GeometryInterface * NovodexPhysicSystem::cookConvex( const float * _verts, int _vertexSize, const int * _indecies, int _indexSize )
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

	NovodexGeometry * novodexGeometry = new NovodexGeometry(nxConvexShape);
	return novodexGeometry;
}
//////////////////////////////////////////////////////////////////////////
GeometryInterface * NovodexPhysicSystem::cookConcave( const float * _verts, int _vertexSize, const int * _indecies, int _indexSize )
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

	NovodexGeometry * novodexGeometry = new NovodexGeometry(nxTriShape);
	return novodexGeometry;
}
//////////////////////////////////////////////////////////////////////////
GeometryInterface * NovodexPhysicSystem::cookConvex( const char * _filename )
{
	NxConvexShapeDesc * nxTriShape = new NxConvexShapeDesc();

	nxTriShape->meshData = m_physicsSDK->createConvexMesh(UserStream(_filename, true));

	NovodexGeometry * novodexGeometry = new NovodexGeometry(nxTriShape);

	return novodexGeometry;
}
//////////////////////////////////////////////////////////////////////////
GeometryInterface * NovodexPhysicSystem::cookConcave( const char * _filename )
{
	NxTriangleMeshShapeDesc * nxTriShape = new NxTriangleMeshShapeDesc();

	nxTriShape->meshData = m_physicsSDK->createTriangleMesh(UserStream(_filename, true));

	NovodexGeometry * novodexGeometry = new NovodexGeometry(nxTriShape);

	return novodexGeometry;
}
//////////////////////////////////////////////////////////////////////////
GeometryInterface * NovodexPhysicSystem::cookBox( float _width, float _height, float _depth )
{
	NxBoxShapeDesc * nxBoxDesc = new NxBoxShapeDesc();
	nxBoxDesc->dimensions = NxVec3( _width * 0.5f, _height * 0.5f, _depth * 0.5f );

	NovodexGeometry * novodexGeometry = new NovodexGeometry(nxBoxDesc);
	return novodexGeometry;
}
//////////////////////////////////////////////////////////////////////////
RigidBodyInterface * NovodexPhysicSystem::createRigidBody( float _density, bool _dynamic, const GeometryInterface * _geometry)
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
	
	RigidBodyInterface * rigidBody = new NovodexBody(_actor);

	return rigidBody;
}
//////////////////////////////////////////////////////////////////////////
void	NovodexPhysicSystem::removeRigidBody( RigidBodyInterface * _rigidBody )
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
void	NovodexPhysicSystem::createJoint(RigidBodyInterface * body0, RigidBodyInterface * body1, float x, float y, float z)
{
	BallSocketJointInterface * joint = new NovodexBallSocketJoint(m_scene);
	joint->init(body0,body1,x,y,z);
	joint->setLimits(3.14f,3.14f);
}
//////////////////////////////////////////////////////////////////////////
void NovodexPhysicSystem::removeGeometry( GeometryInterface * _geom )
{
	delete _geom;
}
//////////////////////////////////////////////////////////////////////////
float NovodexPhysicSystem::rayCast(const char * _name, float * pos, float * dir)
{
	NxRay worldRay;
	worldRay.dir = NxVec3(dir[0], dir[1], dir[2]);
	worldRay.orig = NxVec3(pos[0], pos[1], pos[2]);

	NxRaycastHit hit;

	m_SceneQueryObject->raycastClosestShape(worldRay, NX_STATIC_SHAPES, hit, 0xffffffff, NX_MAX_F32, 0xffffffff, NULL, NULL, NULL);

	m_SceneQueryObject->execute();
	m_SceneQueryObject->finish(true);

	return gHits[0].distance;
	//return hit.distance;
}