#if defined(_MSC_VER) && _MSC_VER <= 0x0600
#pragma warning(disable : 4786)
#endif

#include "model.h"
#include "d3d9RenderSystem.h"


const int Model::STATE_IDLE = 0;
const int Model::STATE_FANCY = 1;
const int Model::STATE_MOTION = 2;

struct Vertex
{
	float pos[3];
	float normal[3];
	float t;
	float s;
};

Model::Model(RenderSystem * _rs)
: m_rs(_rs)
{
	m_calCoreModel = new CalCoreModel("dummy");

	m_state = STATE_IDLE;
	m_motionBlend[0] = 0.6f;
	m_motionBlend[1] = 0.1f;
	m_motionBlend[2] = 0.3f;
	m_animationCount = 0;
	m_meshCount = 0;
	m_renderScale = 1.0f;
	m_lodLevel = 1.0f;

	vertexDecl = m_rs->createVertexDeclaration();

  	IndexData * indexData = m_rs->createIndexData();
	VertexData * vertexData = m_rs->createVertexData();

	pm.setIndexData(indexData);
	pm.setVertexData(vertexData);

	vertexDecl->insert(0, 0, DECLFLOAT3, DECLPOS, 0);
	vertexDecl->insert(0, 12, DECLFLOAT3, DECLNRM, 0);
	vertexDecl->insert(0, 12+12, DECLFLOAT2, DECLTEX, 0);

	vertexData->setVertexDeclaration(vertexDecl);

	vertexData->createVertexBuffer(30000,sizeof(Vertex));
	indexData->createIndexBuffer(30000,0);
}

Model::~Model()
{
	delete vertexDecl;
}

void Model::executeAction(int action)
{
  switch(action)
  {
    case 0:
      m_calModel->getMixer()->executeAction(m_animationId[5], 0.3f, 0.3f);
      break;
    case 1:
      m_calModel->getMixer()->executeAction(m_animationId[6], 0.3f, 0.3f);
      break;
  }
}

float Model::getLodLevel()
{
  return m_lodLevel;
}

void Model::getMotionBlend(float *pMotionBlend)
{
  pMotionBlend[0] = m_motionBlend[0];
  pMotionBlend[1] = m_motionBlend[1];
  pMotionBlend[2] = m_motionBlend[2];
}

float Model::getRenderScale()
{
  return m_renderScale;
}
int Model::getState()
{
  return m_state;
}

int readInt( std::ifstream *file ) 
{
	int x = 0;
	for ( int i = 0; i < 32; i+=8 ) 
	{
		char c;
		file->read ( &c, 1 );
		x += c << i;
 	}
 	return x;
 }

bool Model::onInit(const std::string& strFilename)
{
  // open the model configuration file
  std::ifstream file;
  file.open(strFilename.c_str(), std::ios::in | std::ios::binary);
  if(!file)
  {
    std::cerr << "Failed to open model configuration file '" << strFilename << "'." << std::endl;
    return false;
  }

  // initialize the data path
  std::string strPath = m_path;

  // initialize the animation count
  int animationCount;
  animationCount = 0;

  // parse all lines from the model configuration file
  int line;
  for(line = 1; ; line++)
  {
    // read the next model configuration line
    std::string strBuffer;
    std::getline(file, strBuffer);

    // stop if we reached the end of file
    if(file.eof()) break;

    // check if an error happend while reading from the file
    if(!file)
    {
      std::cerr << "Error while reading from the model configuration file '" << strFilename << "'." << std::endl;
      return false;
    }

    // find the first non-whitespace character
    std::string::size_type pos;
    pos = strBuffer.find_first_not_of(" \t");

    // check for empty lines
    if((pos == std::string::npos) || (strBuffer[pos] == '\n') || (strBuffer[pos] == '\r') || (strBuffer[pos] == 0)) continue;

    // check for comment lines
    if(strBuffer[pos] == '#') continue;

    // get the key
    std::string strKey;
    strKey = strBuffer.substr(pos, strBuffer.find_first_of(" =\t\n\r", pos) - pos);
    pos += strKey.size();

    // get the '=' character
    pos = strBuffer.find_first_not_of(" \t", pos);
    if((pos == std::string::npos) || (strBuffer[pos] != '='))
    {
      std::cerr << strFilename << "(" << line << "): Invalid syntax." << std::endl;
      return false;
    }

    // find the first non-whitespace character after the '=' character
    pos = strBuffer.find_first_not_of(" \t", pos + 1);

    // get the data
    std::string strData;
    strData = strBuffer.substr(pos, strBuffer.find_first_of("\n\r", pos) - pos);

    // handle the model creation
    if(strKey == "scale")
    {
      // set rendering scale factor
      m_renderScale = atof(strData.c_str());
    }
    else if(strKey == "path")
    {
      // set the new path for the data files if one hasn't been set already
      if (m_path == "") strPath = strData;
    }
    else if(strKey == "skeleton")
    {
      // load core skeleton
      std::cout << "Loading skeleton '" << strData << "'..." << std::endl;
      if(!m_calCoreModel->loadCoreSkeleton(strPath + strData))
      {
        CalError::printLastError();
        return false;
      }
    }
    else if(strKey == "animation")
    {
      // load core animation
      std::cout << "Loading animation '" << strData << "'..." << std::endl;
      m_animationId[animationCount] = m_calCoreModel->loadCoreAnimation(strPath + strData);
      if(m_animationId[animationCount] == -1)
      {
        CalError::printLastError();
        return false;
      }

      animationCount++;
    }
    else if(strKey == "mesh")
    {
      // load core mesh
      std::cout << "Loading mesh '" << strData << "'..." << std::endl;
      if(m_calCoreModel->loadCoreMesh(strPath + strData) == -1)
      {
        CalError::printLastError();
        return false;
      }
    }
    else if(strKey == "material")
    {
      // load core material
      std::cout << "Loading material '" << strData << "'..." << std::endl;
      if(m_calCoreModel->loadCoreMaterial(strPath + strData) == -1)
      {
        CalError::printLastError();
        return false;
      }
    }
    else
    {
      std::cerr << strFilename << "(" << line << "): Invalid syntax." << std::endl;
      return false;
    }
  }

  // explicitely close the file
  file.close();

  // load all textures and store the opengl texture id in the corresponding map in the material
  int materialId;
  for(materialId = 0; materialId < m_calCoreModel->getCoreMaterialCount(); materialId++)
  {
    // get the core material
    CalCoreMaterial *pCoreMaterial;
    pCoreMaterial = m_calCoreModel->getCoreMaterial(materialId);

    // loop through all maps of the core material
    int mapId;
    for(mapId = 0; mapId < pCoreMaterial->getMapCount(); mapId++)
    {
      // get the filename of the texture
      std::string strFilename;
      strFilename = pCoreMaterial->getMapFilename(mapId);

      // load the texture from the file
/*      GLuint textureId;
      textureId = loadTexture(strPath + strFilename);

      // store the opengl texture id in the user data of the map
      pCoreMaterial->setMapUserData(mapId, (Cal::UserData)textureId);
	  */
    }
  }

  // make one material thread for each material
  // NOTE: this is not the right way to do it, but this viewer can't do the right
  // mapping without further information on the model etc.
  for(materialId = 0; materialId < m_calCoreModel->getCoreMaterialCount(); materialId++)
  {
    // create the a material thread
    m_calCoreModel->createCoreMaterialThread(materialId);

    // initialize the material thread
    m_calCoreModel->setCoreMaterialId(materialId, 0, materialId);
  }

  // Calculate Bounding Boxes

  m_calCoreModel->getCoreSkeleton()->calculateBoundingBoxes(m_calCoreModel);

  m_calModel = new CalModel(m_calCoreModel);

  // attach all meshes to the model
  int meshId;
  for(meshId = 0; meshId < m_calCoreModel->getCoreMeshCount(); meshId++)
  {
    m_calModel->attachMesh(meshId);
  }

  // set the material set of the whole model
  m_calModel->setMaterialSet(0);

  // set initial animation state
  m_state = STATE_MOTION;
  m_calModel->getMixer()->blendCycle(m_animationId[STATE_MOTION], m_motionBlend[0], 0.0f);
  m_calModel->getMixer()->blendCycle(m_animationId[STATE_MOTION + 1], m_motionBlend[1], 0.0f);
  m_calModel->getMixer()->blendCycle(m_animationId[STATE_MOTION + 2], m_motionBlend[2], 0.0f);

  return true;
}

void Model::renderSkeleton()
{}

void Model::renderBoundingBox()
{}

void Model::renderMesh(bool bWireframe, bool bLight)
{
  CalRenderer *pCalRenderer;
  pCalRenderer = m_calModel->getRenderer();

  if(!pCalRenderer->beginRendering()) return;

  int meshCount = pCalRenderer->getMeshCount();

  int	m_VBCursor = 0;
  int	m_IBCursor = 0;
 
  for(int meshId = 0; meshId < meshCount; meshId++)
  {
    int submeshCount;
    submeshCount = pCalRenderer->getSubmeshCount(meshId);

    int submeshId;
    for(submeshId = 0; submeshId < submeshCount; submeshId++)
    {
      if(pCalRenderer->selectMeshSubmesh(meshId, submeshId))
      {
			float * vertices = 0;

			VertexData * vertexData = pm.getVertexData();
	
			vertexData->lock(vertices,m_VBCursor,pCalRenderer->getVertexCount(),sizeof(Vertex)); 
			int vertexCount = pCalRenderer->getVerticesNormalsAndTexCoords(&vertices[0]);
			vertexData->unlock();

			unsigned short * indecies = 0;

			IndexData * indexData = pm.getIndexData();

			indexData->lock(indecies,m_IBCursor,pCalRenderer->getFaceCount());
			int faceCount = pCalRenderer->getFaces(indecies);
			indexData->unlock();
						
			Direct3d9RenderSystem* d3d9rs =  (Direct3d9RenderSystem*)m_rs;
	
			d3d9rs->_getDevice()->SetTexture(0,(LPDIRECT3DTEXTURE9)pCalRenderer->getMapUserData(0));

			m_rs->drawPrimitive(pm);

			m_VBCursor+=vertexCount;
			m_IBCursor+=faceCount;
      }
    }
  }

  pCalRenderer->endRendering();
  
}

//----------------------------------------------------------------------------//
// Render the model                                                           //
//----------------------------------------------------------------------------//

void Model::onRender()
{
	renderMesh(false, false);
}

void Model::onUpdate(float elapsedSeconds)
{
  m_calModel->update(elapsedSeconds);
}

void Model::onShutdown()
{
  delete m_calModel;
  delete m_calCoreModel;
}

void Model::setLodLevel(float lodLevel)
{
  m_lodLevel = lodLevel;
  m_calModel->setLodLevel(m_lodLevel);
}

void Model::setMotionBlend(float *pMotionBlend, float delay)
{
  m_motionBlend[0] = pMotionBlend[0];
  m_motionBlend[1] = pMotionBlend[1];
  m_motionBlend[2] = pMotionBlend[2];

  m_calModel->getMixer()->clearCycle(m_animationId[STATE_IDLE], delay);
  m_calModel->getMixer()->clearCycle(m_animationId[STATE_FANCY], delay);
  m_calModel->getMixer()->blendCycle(m_animationId[STATE_MOTION], m_motionBlend[0], delay);
  m_calModel->getMixer()->blendCycle(m_animationId[STATE_MOTION + 1], m_motionBlend[1], delay);
  m_calModel->getMixer()->blendCycle(m_animationId[STATE_MOTION + 2], m_motionBlend[2], delay);

  m_state = STATE_MOTION;
}

void Model::setState(int state, float delay)
{
  if(state != m_state)
  {
    if(state == STATE_IDLE)
    {
      m_calModel->getMixer()->blendCycle(m_animationId[STATE_IDLE], 1.0f, delay);
      m_calModel->getMixer()->clearCycle(m_animationId[STATE_FANCY], delay);
      m_calModel->getMixer()->clearCycle(m_animationId[STATE_MOTION], delay);
      m_calModel->getMixer()->clearCycle(m_animationId[STATE_MOTION + 1], delay);
      m_calModel->getMixer()->clearCycle(m_animationId[STATE_MOTION + 2], delay);
      m_state = STATE_IDLE;
    }
    else if(state == STATE_FANCY)
    {
      m_calModel->getMixer()->clearCycle(m_animationId[STATE_IDLE], delay);
      m_calModel->getMixer()->blendCycle(m_animationId[STATE_FANCY], 1.0f, delay);
      m_calModel->getMixer()->clearCycle(m_animationId[STATE_MOTION], delay);
      m_calModel->getMixer()->clearCycle(m_animationId[STATE_MOTION + 1], delay);
      m_calModel->getMixer()->clearCycle(m_animationId[STATE_MOTION + 2], delay);
      m_state = STATE_FANCY;
    }
    else if(state == STATE_MOTION)
    {
      m_calModel->getMixer()->clearCycle(m_animationId[STATE_IDLE], delay);
      m_calModel->getMixer()->clearCycle(m_animationId[STATE_FANCY], delay);
      m_calModel->getMixer()->blendCycle(m_animationId[STATE_MOTION], m_motionBlend[0], delay);
      m_calModel->getMixer()->blendCycle(m_animationId[STATE_MOTION + 1], m_motionBlend[1], delay);
      m_calModel->getMixer()->blendCycle(m_animationId[STATE_MOTION + 2], m_motionBlend[2], delay);
      m_state = STATE_MOTION;
    }
  }
}

void Model::setPath( const std::string& strPath )
{
   m_path = strPath;
}