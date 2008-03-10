#	include "Ogre.h"

#	include <NxPhysics.h>

#	include <NxCooking.h>

#	include "Stream.h"

/*
	_verts - pointer vertices
	_numVertices - number of vertices
	_stride - skip _stride bytes in _verts
	_indecies - pointer to indecies
	_numTriangles - number of triangles
	_is32bit - is 32 bit indecies?
*/

void cookConcave( const void * _verts, int _numVertices, int _stride, const void * _indecies, int _numTriangles, bool _is32bit )
{
	NxTriangleMeshDesc triMeshDesc;

	triMeshDesc.numVertices			= _numVertices;
	triMeshDesc.pointStrideBytes	= _stride;
	triMeshDesc.points				= _verts;
	triMeshDesc.numTriangles		= _numTriangles;
	triMeshDesc.triangles			= _indecies;

	if(_is32bit == true)
	{
		triMeshDesc.triangleStrideBytes	= 3 * sizeof(unsigned int);
		triMeshDesc.flags				= 0;
	}
	else
	{
		triMeshDesc.triangleStrideBytes	= 3 * sizeof(unsigned short);
		triMeshDesc.flags				= NX_CF_16_BIT_INDICES;
	}

	NxInitCooking();

	bool status = NxCookTriangleMesh(triMeshDesc, UserStream("level.bin", false));

	assert(status);

	NxCloseCooking();
}

/*
	_vd - vertex data
	_id - index data
*/

void convert(Ogre::VertexData * _vd, Ogre::IndexData * _id)
{
	Ogre::HardwareVertexBufferSharedPtr vertexBuffer = _vd->vertexBufferBinding->getBuffer(0);
	Ogre::HardwareIndexBufferSharedPtr indexBuffer = _id->indexBuffer;

	int stride = _vd->vertexDeclaration->getVertexSize(0);
	int vertexCount = _vd->vertexCount;
	int numTriangles = _id->indexCount / 3;

	bool use32bitIndexes =
		indexBuffer->getType() == Ogre::HardwareIndexBuffer::IT_32BIT;

	void * vertices = vertexBuffer->lock(0,0,Ogre::HardwareBuffer::HBL_NORMAL);
	void * indecies = indexBuffer->lock(0,0,Ogre::HardwareBuffer::HBL_NORMAL);

	cookConcave(vertices,vertexCount,stride,indecies,numTriangles,use32bitIndexes);

	indexBuffer->unlock();
	vertexBuffer->unlock();
}

void	main()
{
	Ogre::Root * m_root = new Ogre::Root("","","");
	m_root->loadPlugin( "E:/Menge/bin_d/plugins/RenderSystem_GL_d.dll" );

	Ogre::RenderSystem * m_renderSys = m_root->getAvailableRenderers()->at( 0 );
	m_root->setRenderSystem( m_renderSys );
	m_root->initialise( false );

	Ogre::ResourceGroupManager * resourceGroupMgr = 
		Ogre::ResourceGroupManager::getSingletonPtr();

	Ogre::String _path(".");
	resourceGroupMgr->addResourceLocation( _path, "FileSystem", "Default", false );

	resourceGroupMgr->initialiseResourceGroup("Default");

	Ogre::RenderWindow * m_renderWindow = m_root->createRenderWindow( "Menge", 320, 240, 0 );

	Ogre::MeshManager::getSingleton().load("col.mesh","Default");

	Ogre::MeshPtr mesh = (Ogre::MeshPtr)Ogre::MeshManager::getSingleton().getByName("col.mesh");

	assert(mesh->getSubMeshNameMap().size() == 1);

	Ogre::SubMesh * sm = mesh->getSubMesh(0);

	convert(sm->vertexData, sm->indexData);
}