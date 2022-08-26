#include <cstdlib>
#include <ctime>
#include <iostream>
#include "barnsley.h"

const int ESC = 27;

// -----------------------------------------------------------------------------

Application::Application()
	: m_FieldOfViewY			(60.0f)			
	, m_Position				{ 0.0f, 0.0f, 0.0f }
	, m_ViewMatrix				{}
	, m_ProjectionMatrix		{}
	, m_TiltFactorR				(0.04f)
	, m_TiltFactorSummand		(0.0005f)
	, m_VerticalResolution		(1080)
	, m_HorizontalResolution	(1080)
	, m_GenerateWithRandomSeed	(false)
	, m_TiltFern				(true)
	, m_pVSPerFrameConstants	(nullptr)
	, m_pVSPerObjectConstants	(nullptr)
	, m_pPSPerObjectConstants	(nullptr)
	, m_pVertexShader			(nullptr)
	, m_pPixelShader			(nullptr)
	, m_pMaterial				(nullptr)
	, m_pMesh					(nullptr)
{
	srand(static_cast <unsigned> (time(0)));
}

// -----------------------------------------------------------------------------

Application::~Application()
{
}

// -----------------------------------------------------------------------------

bool Application::InternOnStartup()
{
	std::cout << "Controls:" << std::endl;
	std::cout << "R = use random seed for generation" << std::endl;
	std::cout << "T = Stop or Start Tilting of the fern" << std::endl;
	std::cout << "ESC = Exit Application" << std::endl;
	std::cout << "-----------------------------------------" << std::endl;
	std::cout << "Resize the window on startup for better resolution" << std::endl;
	return true;
}
 
// -----------------------------------------------------------------------------

bool Application::InternOnCreateConstantBuffers()
{
	CreateConstantBuffer(sizeof(VSPerFrameConstants), &m_pVSPerFrameConstants);
	CreateConstantBuffer(sizeof(VSPerObjectConstants), &m_pVSPerObjectConstants);
	CreateConstantBuffer(sizeof(PSPerObjectConstants), &m_pPSPerObjectConstants);

	return true;
}

// -----------------------------------------------------------------------------

bool Application::InternOnReleaseConstantBuffers()
{
	ReleaseConstantBuffer(m_pVSPerFrameConstants);
	ReleaseConstantBuffer(m_pVSPerObjectConstants);
	ReleaseConstantBuffer(m_pPSPerObjectConstants);

	return true;
}

// -----------------------------------------------------------------------------

bool Application::InternOnCreateShader()
{
	CreateVertexShader("..\\data\\shader\\barnsley.hlsl", "VSMain", &m_pVertexShader);
	CreatePixelShader("..\\data\\shader\\barnsley.hlsl", "PSMain", &m_pPixelShader);

	return true;
}

// -----------------------------------------------------------------------------

bool Application::InternOnReleaseShader()
{
	ReleaseVertexShader(m_pVertexShader);
	ReleasePixelShader(m_pPixelShader);

	return true;
}

// -----------------------------------------------------------------------------

bool Application::InternOnCreateMaterials()
{
	SMaterialInfo Info;

	Info.m_NumberOfTextures = 0;
	Info.m_NumberOfVertexConstantBuffers = 2;
	Info.m_pVertexConstantBuffers[0] = m_pVSPerFrameConstants;
	Info.m_pVertexConstantBuffers[1] = m_pVSPerObjectConstants;
	Info.m_NumberOfPixelConstantBuffers = 1;
	Info.m_pPixelConstantBuffers[0] = m_pPSPerObjectConstants;
	Info.m_pVertexShader = m_pVertexShader;
	Info.m_pPixelShader = m_pPixelShader;
	Info.m_NumberOfInputElements = 2;
	Info.m_InputElements[0].m_Type = SInputElement::Float3;
	Info.m_InputElements[0].m_pName = "OSPOSITION";
	Info.m_InputElements[1].m_Type = SInputElement::Float2;
	Info.m_InputElements[1].m_pName = "POSITION_NORMED";

	CreateMaterial(Info, &m_pMaterial);

	return true;
}

// -----------------------------------------------------------------------------

bool Application::InternOnReleaseMaterials()
{
	ReleaseMaterial(m_pMaterial);

	return true;
}

// -----------------------------------------------------------------------------

bool Application::InternOnCreateMeshes()
{
	float Vertices[][3 + 2] =
	{
		// X      Y     Z         U      V
		{ -8.0f, -8.0f, 0.0f,  -4.0f, -4.0f },
		{  8.0f, -8.0f, 0.0f,   4.0f, -4.0f },
		{  8.0f,  8.0f, 0.0f,   4.0f,  4.0f },
		{ -8.0f,  8.0f, 0.0f,  -4.0f,  4.0f },
	};

	int Indices[][3] =
	{
		{ 0, 1, 2 }, // Triangle 0
		{ 0, 2, 3 }, // Triangle 1
	};

	SMeshInfo Info;

	Info.m_pVertices = &Vertices[0][0];
	Info.m_NumberOfVertices = 4;
	Info.m_pIndices = &Indices[0][0];
	Info.m_NumberOfIndices = 6;
	Info.m_pMaterial = m_pMaterial;

	CreateMesh(Info, &m_pMesh);

	return true;
}

// -----------------------------------------------------------------------------

bool Application::InternOnReleaseMeshes()
{
	ReleaseMesh(m_pMesh);

	return true;
}

// -----------------------------------------------------------------------------

bool Application::InternOnResize(int _Width, int _Height)
{
	float AspectRatio = static_cast<float>(_Width) / static_cast<float>(_Height);
	m_HorizontalResolution = _Width;
	m_VerticalResolution = _Height;
	GetProjectionMatrix(m_FieldOfViewY, AspectRatio, 0.01f, 1000.0f, m_ProjectionMatrix);

	return true;
}

// -----------------------------------------------------------------------------

bool Application::InternOnUpdate()
{
	float Eye[3] = { 1.5f, 1.0f, -2.0f };
	float At [3] = { 1.5f, 1.0f,  0.0f };
	float Up [3] = { 0.0f, 1.0f,  0.0f };

	GetViewMatrix(Eye, At, Up, m_ViewMatrix);

	return true;
}

// -----------------------------------------------------------------------------

bool Application::InternOnFrame()
{
	VSPerFrameConstants PerFrameConstantsVS;
	MulMatrix(m_ViewMatrix, m_ProjectionMatrix, PerFrameConstantsVS.m_VSViewProjectionMatrix);
	UploadConstantBuffer(&PerFrameConstantsVS, m_pVSPerFrameConstants);

	VSPerObjectConstants PerObjectConstantsVS;
	GetTranslationMatrix(m_Position[0], m_Position[1], m_Position[2], PerObjectConstantsVS.m_VSWorldMatrix);
	UploadConstantBuffer(&PerObjectConstantsVS, m_pVSPerObjectConstants);


	PSPerObjectConstants PerObjectConstantsPS;

	float random=50.0f;

	if (m_GenerateWithRandomSeed) random = rand() / static_cast<float>(RAND_MAX);

	PerObjectConstantsPS.m_PSRandNr = random;
	
	PerObjectConstantsPS.m_PSVerticalResolution = m_VerticalResolution;
	PerObjectConstantsPS.m_PSHorizontalResolution = m_HorizontalResolution;
	
	if (m_TiltFern)
	{
		if (m_TiltFactorR <= -0.09f) m_TiltFactorSummand = 0.0005f;
		if (m_TiltFactorR >= 0.09f) m_TiltFactorSummand = -0.0005f;
	
		m_TiltFactorR += m_TiltFactorSummand;
	}
	PerObjectConstantsPS.m_PSTiltFactorR = m_TiltFactorR;

	UploadConstantBuffer(&PerObjectConstantsPS, m_pPSPerObjectConstants);


	DrawMesh(m_pMesh);

	return true;
}

// -----------------------------------------------------------------------------

bool Application::InternOnKeyEvent(unsigned int _Key, bool _IsKeyDown, bool _IsAltDown)
{
	if (_IsKeyDown)
	{
		switch (_Key)
		{
		case 'R': 
			m_GenerateWithRandomSeed = !m_GenerateWithRandomSeed;
			break;
		case 'T': 
			m_TiltFern = !m_TiltFern;
			break;
		case ESC: //ESC key -> Nr 27
			StopApplication();
			break;
		}
	}
	return true;
}



// -----------------------------------------------------------------------------

void main()
{
	Application Application;

	RunApplication(1080, 1080, "Barnsley's Fern", &Application);
}
