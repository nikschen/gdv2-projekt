#pragma once

#include "yoshix.h"

using namespace gfx;

class Application : public IApplication
{
public:

	Application();
	virtual ~Application();

private:

	struct VSPerFrameConstants
	{
		float m_VSViewProjectionMatrix[16];
	};

	struct VSPerObjectConstants
	{
		float m_VSWorldMatrix[16];
	};

	struct PSPerObjectConstants
	{
		float		m_PSRandNr;
		int			m_PSVerticalResolution;
		int			m_PSHorizontalResolution;
		float		m_PSTiltFactorR;
	};

	float   m_FieldOfViewY;
	float   m_Position[3];
	float   m_ProjectionMatrix[16];
	float	m_TiltFactorR;
	float	m_TiltFactorSummand;
	float   m_ViewMatrix[16];
	int		m_VerticalResolution;
	int		m_HorizontalResolution;
	bool	m_GenerateWithRandomSeed;
	bool	m_TiltFern;
	BHandle m_pMaterial;
	BHandle m_pMesh;
	BHandle m_pPixelShader;
	BHandle m_pVertexShader;
	BHandle m_pPSPerObjectConstants;
	BHandle m_pVSPerFrameConstants;
	BHandle m_pVSPerObjectConstants;

	virtual bool InternOnCreateConstantBuffers();
	virtual bool InternOnReleaseConstantBuffers();
	virtual bool InternOnCreateShader();
	virtual bool InternOnReleaseShader();
	virtual bool InternOnCreateMaterials();
	virtual bool InternOnReleaseMaterials();
	virtual bool InternOnCreateMeshes();
	virtual bool InternOnReleaseMeshes();
	virtual bool InternOnResize(int _Width, int _Height);
	virtual bool InternOnUpdate();
	virtual bool InternOnFrame();
	virtual bool InternOnKeyEvent(unsigned int _Key, bool _IsKeyDown, bool _IsAltDown);
	virtual bool InternOnStartup();
};