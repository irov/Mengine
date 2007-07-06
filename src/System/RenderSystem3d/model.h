#	pragma once
#	include "RenderSystem3dInterface.h"

#	include <windows.h>
#	include <iostream>
#	include <fstream>
#	include <string>

#	include "cal3d/cal3d.h"

class Model
{
public:
	static const int STATE_IDLE;
	static const int STATE_FANCY;
	static const int STATE_MOTION;

protected:
	int m_state;
	CalCoreModel* m_calCoreModel;
	CalModel* m_calModel;
	int m_animationId[16];
	int m_animationCount;
	int m_meshId[32];
	int m_meshCount;
	int m_textureCount;
	float m_motionBlend[3];
	float m_renderScale;
	float m_lodLevel;
	std::string m_path;

	//
	RenderSystem * m_rs;
	VertexDeclaration * vertexDecl;
	PrimitiveData pm;

public:
	Model(RenderSystem * _rs);
	virtual ~Model();

public:
	void executeAction(int action);
	float getLodLevel();
	void getMotionBlend(float *pMotionBlend);
	float getRenderScale();
	int getState();
	bool onInit(const std::string& strFilename);
	void onRender();
	void onShutdown();
	void onUpdate(float elapsedSeconds);
	void setLodLevel(float lodLevel);
	void setMotionBlend(float *pMotionBlend, float delay);
	void setState(int state, float delay);
	void setPath( const std::string& strPath );
protected:
	void renderMesh(bool bWireframe, bool bLight);
	void renderSkeleton();
	void renderBoundingBox();
};