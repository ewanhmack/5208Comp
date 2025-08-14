//------------Include the Application Header File----------------------------
#include "LJMULevelDemo.h"

//------------DX TK AND STD/STL Includes-------------------------------------
#include <sstream>

//------------Include Hieroglyph Engine Files--------------------------------

//Include the Logging System
#include "Log.h"

//Include the Event System
#include "EventManager.h"
#include "EvtFrameStart.h"
#include "EvtChar.h"
#include "EvtKeyUp.h"
#include "EvtKeyDown.h"
#include "ScriptManager.h"

//Include the DirectX Rendering Components
#include "PipelineManagerDX11.h"
#include "BlendStateConfigDX11.h"
#include "BufferConfigDX11.h"
#include "DepthStencilStateConfigDX11.h"
#include "RasterizerStateConfigDX11.h"
#include "SwapChainConfigDX11.h"
#include "Texture2dConfigDX11.h"
#include "MaterialGeneratorDX11.h"

#include "FirstPersonCamera.h"

#include <SamplerStateConfigDX11.h>

#include "LJMUMeshOBJ.h"
#include <FileSystem.h>
#include "vector"

#include "LJMUSpriteOverlay.h"

//Add a Using Directive to avoid typing Glyph3 for basic constructs
using namespace Glyph3;
//Include our own application Namespace
using namespace LJMUDX;

LJMULevelDemo AppInstance;

//---------CONSTRUCTORS-------------------------------------------------------

///////////////////////////////////////
//
///////////////////////////////////////
LJMULevelDemo::LJMULevelDemo() :
	m_pRender_text(nullptr),
	m_pRenderView(nullptr),
	m_pCamera(nullptr),
	m_pRenderer11(nullptr),
	m_pWindow(nullptr),
	m_iSwapChain(0),
	m_DepthTarget(nullptr),
	m_RenderTarget(nullptr)
{

}

//---------METHODS------------------------------------------------------------

//////////////////////////////////////
// Get the Window Name of this Application
//////////////////////////////////////
std::wstring LJMULevelDemo::GetName()
{
	return(std::wstring(L"5108COMP: Coursework Template"));
}

/////////////////////////////////////
// Assemble our Input Layouts for this
// Stage of the Pipeline.
/////////////////////////////////////
void LJMULevelDemo::inputAssemblyStage()
{
	//-----SETUP OUR GEOMETRY FOR THIS SCENE-----------------------------------------
	//Adding a platform
	m_racetrackTexture = RendererDX11::Get()->LoadTexture(L"racetrack.png");
	m_platformActor = new LJMUGeometryActor();
	m_platformActor->SetColor(Vector4f(0.6f, 0.3f, 0.1f, 1.0f));
	m_platformActor->DrawRect(Vector3f(0.0f, 0.0f, 0.0f),
		Vector3f(1.0f, 0.0f, 0.0f),
		Vector3f(0.0f, 0.0f, -1.0f),
		Vector2f(m_platformWidth, m_platformLength));
	//m_platformActor->UseSolidMaterial();
	//m_platformActor->UseTexturedMaterial(m_racetrackTexture);
	setupMaterialProperties(m_platformActor->m_pLitTextureMaterial);
	setLights2Material(m_platformActor->m_pLitTextureMaterial);
	m_platformActor->UseLitTextureMaterial(m_racetrackTexture);
	m_platformActor->GetNode()->Position() = Vector3f(0.0f, 0.0f, 0.0f);

	this->m_pScene->AddActor(m_platformActor);

	//Adding a light
	PointLight* tlight = new PointLight();
	tlight->GetSpecular() = Vector4f(1.2f, 1.2f, 1.2, 1);
	tlight->GetDiffuse() = Vector4f(1.2f, 1.2f, 1.2, 1);
	tlight->GetAmbient() = Vector4f(1.2f, 1.2f, 1.2, 1);
	tlight->GetNode()->Position() = Vector3f(-128.0f, 128.0f, -128.0f);
	m_pScene->AddLight(tlight);

	//Adding a car
	m_carActor = new LJMUGeometryActor();
	Vector4f base_colour = Vector4f(1, 1, 1, 1);
	m_carTexture = RendererDX11::Get()->LoadTexture(L"Lamborginhi_Aventador_diffuse.png");
	BasicMeshPtr car_Geometry = this->generateOBJMesh(L"Lamborghini_Aventador_triangles.obj", base_colour);
	m_carActor = new LJMUGeometryActor();
	m_carActor->GetBody()->SetGeometry(car_Geometry);
	m_carActor->UseLitTextureMaterial(m_carTexture);
	m_carActor->GetNode()->Position() = Vector3f(75.0f, 0.0f, 75.0f);
	m_carActor->GetNode()->Scale() = Vector3f(0.05f, 0.05f, 0.05f);
	this->m_pScene->AddActor(m_carActor);


	//this->m_pScene->AddActor(m_carActor);

	//Adding a skybox
	m_SkylineDayTexture = RendererDX11::Get()->LoadTexture(L"Sky1.png");
	m_SkylineNightTexture = RendererDX11::Get()->LoadTexture(L"Sky0.png");
	m_SkyTexture = m_SkylineDayTexture;
	m_SkyLine = new LJMUGeometryActor();
	m_SkyLine->DrawSphere(Vector3f(20.0f, 20.0f, 20.0f), 700, 80, 80);
	m_SkyLine->UseSolidMaterial();
	m_SkyLine->UseSkyBoxMaterial(m_SkyTexture);
	m_SkyLine->GetNode()->Position() = Vector3f(0.0f, -20.0f, 0.0f);
	this->m_pScene->AddActor(m_SkyLine);

	const int NumberOfCheckpoints = 85;
	Vector3f checkpointCoords[NumberOfCheckpoints] = {
	{ -9.44f, 0.0f, -130.24f },
	{ 182.56f, 0.0f, -8.80f },
	{ 190.72f, 0.0f, 4.16f },
	{ 200.32f, 0.0f, 16.64f },
	{ 208.00f, 0.0f, 32.00f },
	{ 214.24f, 0.0f, 48.80f },
	{ 216.64f, 0.0f, 67.04f },
	{ 216.64f, 0.0f, 84.80f },
	{ 212.80f, 0.0f, 103.04f },
	{ 206.08f, 0.0f, 119.36f },
	{ 197.44f, 0.0f, 133.28f },
	{ 185.44f, 0.0f, 146.24f },
	{ 170.56f, 0.0f, 157.28f },
	{ 154.24f, 0.0f, 165.92f },
	{ 137.44f, 0.0f, 170.72f },
	{ 118.72f, 0.0f, 173.12f },
	{ 96.64f, 0.0f, 173.12f },
	{ 77.44f, 0.0f, 168.32f },
	{ 59.20f, 0.0f, 160.16f },
	{ -128.48f, 0.0f, 38.24f },
	{ -146.24f, 0.0f, 20.00f },
	{ -156.80f, 0.0f, 4.16f },
	{ -164.96f, 0.0f, -12.16f },
	{ -187.04f, 0.0f, -111.04f },
	{ -186.56f, 0.0f, -135.04f },
	{ -183.68f, 0.0f, -154.72f },
	{ -176.96f, 0.0f, -171.04f },
	{ -171.20f, 0.0f, -180.64f },
	{ -148.64f, 0.0f, -195.04f },
	{ -136.64f, 0.0f, -196.00f },
	{ -127.52f, 0.0f, -195.04f },
	{ -118.40f, 0.0f, -191.20f },
	{ -111.20f, 0.0f, -183.52f },
	{ -106.88f, 0.0f, -176.32f },
	{ -106.40f, 0.0f, -165.76f },
	{ -107.84f, 0.0f, -153.76f },
	{ -111.68f, 0.0f, -142.72f },
	{ -118.40f, 0.0f, -131.68f },
	{ -126.08f, 0.0f, -120.64f },
	{ -133.76f, 0.0f, -107.20f },
	{ -139.04f, 0.0f, -93.28f },
	{ -139.52f, 0.0f, -79.36f },
	{ -139.04f, 0.0f, -66.88f },
	{ -136.64f, 0.0f, -53.44f },
	{ -134.40f, 0.0f, -40.48f },
	{ -132.48f, 0.0f, -27.04f },
	{ -127.04f, 0.0f, -14.08f },
	{ -117.76f, 0.0f, -0.16f },
	{ 83.20f, 0.0f, 135.20f },
	{ 92.80f, 0.0f, 139.04f },
	{ 108.64f, 0.0f, 140.96f },
	{ 123.52f, 0.0f, 139.52f },
	{ 139.36f, 0.0f, 134.72f },
	{ 153.76f, 0.0f, 127.52f },
	{ 163.36f, 0.0f, 117.44f },
	{ 170.08f, 0.0f, 107.36f },
	{ 174.88f, 0.0f, 93.44f },
	{ 177.28f, 0.0f, 77.28f },
	{ 176.80f, 0.0f, 65.12f },
	{ 173.44f, 0.0f, 50.24f },
	{ 165.76f, 0.0f, 34.88f },
	{ 156.16f, 0.0f, 22.40f },
	{ 143.68f, 0.0f, 12.80f },
	{ 129.76f, 0.0f, 5.60f },
	{ 112.48f, 0.0f, 1.76f },
	{ 90.40f, 0.0f, -0.16f },
	{ 69.28f, 0.0f, 1.76f },
	{ 46.24f, 0.0f, 6.56f },
	{ 28.00f, 0.0f, 9.44f },
	{ 8.32f, 0.0f, 11.36f },
	{ -12.80f, 0.0f, 10.88f },
	{ -39.68f, 0.0f, 6.56f },
	{ -64.16f, 0.0f, -4.48f },
	{ -81.44f, 0.0f, -16.48f },
	{ -94.88f, 0.0f, -32.32f },
	{ -100.64f, 0.0f, -46.72f },
	{ -103.04f, 0.0f, -66.40f },
	{ -100.16f, 0.0f, -83.68f },
	{ -94.40f, 0.0f, -100.48f },
	{ -83.84f, 0.0f, -116.32f },
	{ -70.88f, 0.0f, -125.92f },
	{ -56.00f, 0.0f, -132.16f },
	{ -39.68f, 0.0f, -134.56f },
	{ -27.20f, 0.0f, -134.56f },
	{ -17.12f, 0.0f, -133.12f }
	};

	for (size_t i = 0; i < NumberOfCheckpoints; i++)
	{
		m_checkpoints.push_back(checkpointCoords[i]);
	}

	for (int i = 0; i < m_checkpoints.size(); i++)
	{
		GeometryActor* pathSegmentActor = new GeometryActor();
		pathSegmentActor->SetColor(Vector4f(1.0f, 0.1f, 0.1f, 1.0f));
		int startidx = i;
		int endidx = (i + 1) % m_checkpoints.size();
		Vector3f cylinderStartPoint = m_checkpoints[startidx] + Vector3f(0, 3, 0);
		Vector3f cylinderEndPoint = m_checkpoints[endidx] + Vector3f(0, 3, 0);
		//pathSegmentActor->DrawCylinder(cylinderStartPoint, cylinderEndPoint, 0.4f, 0.4f);
		pathSegmentActor->UseSolidMaterial();
		pathSegmentActor->GetNode()->Position() = Vector3f(0.0f, 0.0f, 0.0f);
		this->m_pScene->AddActor(pathSegmentActor);
	}

	m_currentCheckpointIndex = 0;
	m_nextCheckpointIndex = m_currentCheckpointIndex + 1;

	Matrix3f tstartRotation;

	m_referenceCarDirection = Vector3f(0, 0, 1);
	m_currentCarDirection = m_referenceCarDirection;
	m_targetCarDirection = m_checkpoints[m_nextCheckpointIndex] - m_checkpoints[m_currentCheckpointIndex];
	m_carDistance2NextCheckpoint = m_targetCarDirection.Magnitude();
	m_targetCarDirection.Normalize();

	float angle = acos(m_referenceCarDirection.Dot(m_targetCarDirection));

	Vector3f axis = m_referenceCarDirection.Cross(m_targetCarDirection);
	axis.Normalize();

	tstartRotation.RotationEuler(axis, angle);

	m_carActor->GetNode()->Rotation() = tstartRotation;
	m_carActor->GetNode()->Position() = m_checkpoints[m_currentCheckpointIndex];

	
	m_carAngularSpeed = 2.5f; //speed car takes corners

	//m_carState = 0: Car moving forward
	//m_carState = 1: Car is turning round
	//m_carState = 0;

	//tree
	// Define the trunk length and size
	float treetrunklength = 8.0f;
	float treetrunksize = 1.5f;
	//Get textures
	m_treeStemTexture = RendererDX11::Get()->LoadTexture(L"treetrunk.png");
	m_treeTopTexture = RendererDX11::Get()->LoadTexture(L"leaves.png");


	// Array of tree coordinates
	const int NumberOfTrees = 15;
	Vector3f treeCoords[NumberOfTrees] = {
		{ -24.80f, 0.0f, -56.32f },
		{ 72.00f, 0.0f, -38.56f },
		{ 94.72f, 0.0f, 30.08f },
		{ 139.36f, 0.0f, 78.08f },
		{ 239.52f, 0.0f, 102.08f },
		{ 32.32f, 0.0f, 61.28f },
		{ 96.64f, 0.0f, -98.56f },
		{ 206.08f, 0.0f, -38.56f },
		{ 244.96f, 0.0f, 19.52f },
		{ -65.12f, 0.0f, -173.44f },
		{ -190.88f, 0.0f, -217.12f },
		{ -227.36f, 0.0f, -148.96f },
		{ -208.64f, 0.0f, -6.40f },
		{ -142.40f, 0.0f, 80.00f },
		{ -48.80f, 0.0f, 133.76f }
	};

	// Loop through each tree coordinate
	for (int i = 0; i < NumberOfTrees; i++)
	{
		// Create tree trunk actor
		float treetrunklength = 8.0f;
		float treetrunksize = 1.5f;
		m_treeTrunkActor = new LJMUGeometryActor();
		m_treeTrunkActor->SetColor(Vector4f(0.65f, 0.16f, 0.16f, 1.0f));
		m_treeTrunkActor->DrawBox(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(1.0f, treetrunklength, 1.0f));
		m_treeTrunkActor->GetNode()->Position() = Vector3f(0.0f, treetrunklength * treetrunksize, 0.0f);
		m_treeTrunkActor->UseLitTextureMaterial(m_treeStemTexture);
		m_treeTrunkActor->GetNode()->Scale() = Vector3f(1, 1, 1) * treetrunksize;
		this->m_pScene->AddActor(m_treeTrunkActor);

		m_treeTopActor = new LJMUGeometryActor();
		m_treeTopActor->SetColor(Vector4f(0.0f, 0.5f, 0.0f, 1.0f));
		m_treeTopActor->DrawCylinder(Vector3f(0, 0, 0), Vector3f(0, 12, 0), 6, 0, 5, 20);
		m_treeTopActor->DrawDisc(Vector3f(0, 0, 0), Vector3f(0, -1, 0), 6);
		m_treeTopActor->GetNode()->Position() = Vector3f(0.0f, treetrunklength, 0.0f);
		m_treeTopActor->UseLitTextureMaterial(m_treeTopTexture);
		this->m_pScene->AddActor(m_treeTopActor);


		Glyph3::Node3D* treeTopNode = m_treeTopActor->GetNode();
		m_treeTrunkActor->GetNode()->AttachChild(treeTopNode);

		m_treeTrunkPivot = new Actor();
		m_treeTrunkPivot->GetNode()->AttachChild(m_treeTrunkActor->GetNode());
		m_treeTrunkPivot->GetNode()->Position() = treeCoords[i];
		this->m_pScene->AddActor(m_treeTrunkPivot);

		// Add to the list of tree actors
		m_treeActor.push_back(m_treeTrunkPivot);
	}

	
	//Adding the trex obj
	m_trexTexture = RendererDX11::Get()->LoadTexture(L"trex_diff.png");
	BasicMeshPtr trex_geometry = this->generateOBJMesh(L"trex.obj", base_colour);

	m_trexActor = new LJMUGeometryActor();
	m_trexActor->GetBody()->SetGeometry(trex_geometry);
	m_trexActor->UseLitTextureMaterial(m_trexTexture);
	m_trexActor->GetNode()->Position() = Vector3f(0.0f, 0.0f, -30.0f);
	m_trexActor->GetNode()->Scale() = Vector3f(0.3f, 0.3f, 0.3f);
	this->m_pScene->AddActor(m_trexActor);

	//Adding the reptile obj
	m_lizardTexture = RendererDX11::Get()->LoadTexture(L"reptile_diff.png");
	BasicMeshPtr lizard_geometry = this->generateOBJMesh(L"reptile.obj", base_colour);

	m_lizardActor = new LJMUGeometryActor();
	m_lizardActor->GetBody()->SetGeometry(lizard_geometry);
	m_lizardActor->UseLitTextureMaterial(m_lizardTexture);
	m_lizardActor->GetNode()->Position() = Vector3f(99.52f, 0.0f, -140.80f);
	m_lizardActor->GetNode()->Scale() = Vector3f(0.3f, 0.3f, 0.3f);
	m_lizardActor->GetNode()->Rotation() * 20;
	this->m_pScene->AddActor(m_lizardActor);

	//Adding the bull obj
	m_bullTexture = RendererDX11::Get()->LoadTexture(L"bull_diff.png");
	BasicMeshPtr bull_geometry = this->generateOBJMesh(L"bull.obj", base_colour);

	m_bullActor = new LJMUGeometryActor();
	m_bullActor->GetBody()->SetGeometry(bull_geometry);
	m_bullActor->UseLitTextureMaterial(m_bullTexture);
	m_bullActor->GetNode()->Position() = Vector3f(-144.80f, 0.0f, -153.28f);
	m_bullActor->GetNode()->Scale() = Vector3f(0.3f, 0.3f, 0.3f);
	this->m_pScene->AddActor(m_bullActor);


}

////////////////////////////////////
// Initialise our DirectX 3D Scene
////////////////////////////////////
void LJMULevelDemo::Initialize()
{
	setupLightSources();
	//Call the Input Assembly Stage to setup the layout of our Engine Objects
	this->inputAssemblyStage();

	this->m_pCamera = new FirstPersonCamera();
	this->m_pCamera->SetEventManager(&this->EvtManager);

	//Vector3f tcamerapos(0, 0, 0);
	//Vector3f tcamerapos(0, 150, -350);
	Vector3f tcamerapos(0, 250, 0);
	this->m_pCamera->Spatial().SetTranslation(tcamerapos);
	this->m_pCamera->Spatial().RotateXBy(20 * DEG_TO_RAD);


	this->m_pRenderView = new ViewPerspective(*this->m_pRenderer11,
		this->m_RenderTarget, this->m_DepthTarget);
	this->m_pRenderView->SetBackColor(Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
	this->m_pCamera->SetCameraView(this->m_pRenderView);

	this->m_pRender_text = new LJMUTextOverlay(*this->m_pRenderer11,
		this->m_RenderTarget,
		std::wstring(L"Cambria"),
		25);

	this->m_pCamera->SetOverlayView(this->m_pRender_text);


	this->m_pCamera->SetProjectionParams(0.1f, 1000.0f, m_iscreenWidth / m_iscreenHeight,
		static_cast<float>(GLYPH_PI) / 2.0f);

	this->m_pScene->AddCamera(this->m_pCamera);

	//Text

}

///////////////////////////////////
// Update the State of our Game and 
// Output the Results to Screen (Render)
/////////////////////////////////// 
void LJMULevelDemo::Update()
{
	this->m_pTimer->Update();
	EvtManager.ProcessEvent(EvtFrameStartPtr(new EvtFrameStart(this->m_pTimer->Elapsed())));

	float tpf = m_pTimer->Elapsed();

	if(tpf > 10.0f / 60.0f)
	{
		tpf = 1 / 60.0f;
	}

	m_totalPlayTime += tpf;

	//Calculate m_targetCarDirection, the direction the car NEED to face
	m_targetCarDirection = m_checkpoints[m_nextCheckpointIndex] - m_carActor->GetNode()->Position();
	//Calculate distance2NextCheckpoint, how far the car is CURRENTLY to the next checkpoint
	float distance2NextCheckpoint = m_targetCarDirection.Magnitude();
	//Make m_targetCarDirection a unit vector, because we are going to perform DOT and CROSS products using it later
	m_targetCarDirection.Normalize();
	//Compare distance2NextCheckpoint to m_carDistance2NextCheckpoint
	//m_carDistnace2NextCheckpoint is the PREVIOUS distance of the car to the next checkpoint
	if (distance2NextCheckpoint > m_carDistance2NextCheckpoint)
	{
		//This means the car passed the checkpoint, so it is time to move to the next checkpoint
		//by making the TARGET/NEXT checkpoint the CURRENT checkpoint
		//and the one after that as the TARGET/NEXT checkpoint
		m_currentCheckpointIndex++;
		m_nextCheckpointIndex++;
		m_currentCheckpointIndex = m_currentCheckpointIndex % m_checkpoints.size();
		m_nextCheckpointIndex = m_nextCheckpointIndex % m_checkpoints.size();

		//Snap the car to the CURRENT (or was last) checkpoint
		m_carActor->GetNode()->Position() = m_checkpoints[m_currentCheckpointIndex];

		//Recalculate m_targetCarDirection, the direction the car NEED to face
		m_targetCarDirection = m_checkpoints[m_nextCheckpointIndex] - m_carActor->GetNode()->Position();

		//Recalculate distance2NextCheckpoint, how far the car is CURRENTLY to the next checkpoint
		distance2NextCheckpoint = m_targetCarDirection.Magnitude();

		//Make m_targetCarDirection a unit vector again
		m_targetCarDirection.Normalize();

		//Set the cars CURRENT distance to the next checkpoint as the last distance
		m_carDistance2NextCheckpoint = distance2NextCheckpoint;		
	}
	// Calculate the angle between the current car direction and where the car needs to face
	float angle = acos(m_currentCarDirection.Dot(m_targetCarDirection));

	// Next we are going to rotate the car by speed of rotation of m_carAngularSpeed
	// Compare the angle with the angular increment
	if (angle < m_carAngularSpeed * tpf)
	{
		// If the angle is smaller than the angular increment, snap the rotation to that
		m_currentCarDirection = m_targetCarDirection;
		angle = acos(m_referenceCarDirection.Dot(m_targetCarDirection));

		Vector3f axis = m_referenceCarDirection.Cross(m_currentCarDirection);
		axis.Normalize();

		// Snap the car rotation
		Matrix3f tstartRotation;
		tstartRotation.RotationEuler(axis, angle);
		m_carActor->GetNode()->Rotation() = tstartRotation;
	}
	else
	{
		// Otherwise calculate the new rotation angle
		angle = angle + m_carAngularSpeed * tpf;

		Vector3f axis = m_currentCarDirection.Cross(m_targetCarDirection);
		axis.Normalize();

		// Rotate the car
		Matrix3f tstartRotation;
		tstartRotation.RotationEuler(axis, m_carAngularSpeed * tpf);
		m_carActor->GetNode()->Rotation() *= tstartRotation;
	}
	// Calculate the car's CURRENT direction vector
	m_currentCarDirection = m_carActor->GetNode()->Rotation() * m_referenceCarDirection;
	// Make that unit vector
	m_currentCarDirection.Normalize();
	// Move the car in that direction with m_carLinearSpeed increment
	m_carActor->GetNode()->Position() = m_carActor->GetNode()->Position() + m_currentCarDirection * m_carLinearSpeed * tpf;
	// Store the distance2NextCheckpoint to m_carDistance2NextCheckpoint
	m_carDistance2NextCheckpoint = distance2NextCheckpoint;

	// In the next loop, distance2NextCheckpoint will be recalculated based on the new car's position
	// and m_carDistance2NextCheckpoint contains the old car's position
	// and the process repeats
	for (size_t i = 0; i < m_treeActor.size(); i++)
	{
		m_totalPlayTime += tpf;
		Matrix3f trotation;
		float rotationSpeed = 0.3;
		float maxswayangle = 10.0 / 360.0 * GLYPH_PI; // 10 degrees

		// Compute the sway angle
		trotation.RotationZ(cos(m_totalPlayTime * rotationSpeed) * maxswayangle);

		// Apply the rotation to the pivot node
		m_treeActor[i]->GetNode()->Rotation() = trotation;
	}

	updateLightSources();
	applyLights2AllMaterials();

	//Text rendering
	// Text Rendering: Display m_carActor details
	Matrix4f textPosition = Matrix4f::Identity();
	float textX = 30.0f;
	float textY = 30.0f;
	textPosition.SetTranslation(Vector3f(textX, textY, 0.0f));

	Vector3f carPosition = m_carActor->GetNode()->Position();
	std::wstringstream carInfo;
	carInfo << L"Car Position: [";
	carInfo << std::fixed;
	carInfo << carPosition.x << L", " << carPosition.y << L", " << carPosition.z;
	carInfo << L"]\n";

	carInfo << L"Speed: " << std::fixed << m_carLinearSpeed << L"\n";

	carInfo << L"Direction: [";
	carInfo << m_currentCarDirection.x << L", " << m_currentCarDirection.y << L", " << m_currentCarDirection.z;
	carInfo << L"]\n";

	carInfo << L"FPS: " << outputFPSInfo();
	carInfo << L"tpf " << m_totalPlayTime;
	m_pRender_text->writeText(carInfo.str(), textPosition, Vector4f(1.0f, 1.0f, 1.0f, 1.0f));

	// Render Car Sprite below text
	

	//Day - Night Cycle
	// Update DirectionalLightColour based on m_totalPlayTime
{
        float cycleDuration = 600.0f; // Total time for a complete cycle (day -> night -> day)
        float progress = fmod(m_totalPlayTime, cycleDuration);

        if (progress <= 120.0f)
        {
            // Day phase
            DirectionalLightColour = Vector4f(0.07f, 0.07f, 0.07f, 1.0f);
            m_SkyTexture = m_SkylineDayTexture;
        }
        else if (progress <= 240.0f)
        {
            // Transition to night
            float normalizedProgress = (progress - 120.0f) / 120.0f;
            DirectionalLightColour = Vector4f(
                0.07f - normalizedProgress * (0.07f - 0.002f),
                0.07f - normalizedProgress * (0.07f - 0.002f),
                0.07f - normalizedProgress * (0.07f - 0.002f),
                1.0f
            );
            m_SkyTexture = m_SkylineDayTexture; // Keep day texture during transition
        }
        else if (progress <= 360.0f)
        {
            // Night phase
            DirectionalLightColour = Vector4f(0.002f, 0.002f, 0.002f, 1.0f);
            m_SkyTexture = m_SkylineNightTexture;
        }
        else if (progress <= 480.0f)
        {
            // Transition to day
            float normalizedProgress = (progress - 360.0f) / 120.0f;
            DirectionalLightColour = Vector4f(
                0.002f + normalizedProgress * (0.07f - 0.002f),
                0.002f + normalizedProgress * (0.07f - 0.002f),
                0.002f + normalizedProgress * (0.07f - 0.002f),
                1.0f
            );
            m_SkyTexture = m_SkylineNightTexture; // Keep night texture during transition
        }
        else
        {
            // Day phase
            DirectionalLightColour = Vector4f(0.07f, 0.07f, 0.07f, 1.0f);
            m_SkyTexture = m_SkylineDayTexture;
        }
    }
	
	//----------START RENDERING--------------------------------------------------------------

	this->m_pScene->Update(m_pTimer->Elapsed());
	this->m_pScene->Render(this->m_pRenderer11);

	//--------END RENDERING-------------------------------------------------------------
	this->m_pRenderer11->Present(this->m_pWindow->GetHandle(), this->m_pWindow->GetSwapChain());
}

///////////////////////////////////
// Configure the DirectX 11 Programmable
// Pipeline Stages and Create the Window
// Calls 
///////////////////////////////////
bool LJMULevelDemo::ConfigureEngineComponents()
{

	// Set the render window parameters and initialize the window
	this->m_pWindow = new Win32RenderWindow();
	this->m_pWindow->SetPosition(25, 25);
	this->m_pWindow->SetSize(m_iscreenWidth, m_iscreenHeight);
	this->m_pWindow->SetCaption(this->GetName());
	this->m_pWindow->Initialize(this);


	// Create the renderer and initialize it for the desired device
	// type and feature level.
	this->m_pRenderer11 = new RendererDX11();

	if (!this->m_pRenderer11->Initialize(D3D_DRIVER_TYPE_HARDWARE, D3D_FEATURE_LEVEL_11_0))
	{
		Log::Get().Write(L"Could not create hardware device, trying to create the reference device...");

		if (!this->m_pRenderer11->Initialize(D3D_DRIVER_TYPE_REFERENCE, D3D_FEATURE_LEVEL_10_0))
		{
			ShowWindow(this->m_pWindow->GetHandle(), SW_HIDE);
			MessageBox(this->m_pWindow->GetHandle(), L"Could not create a hardware or software Direct3D 11 device!",
				L"5108COMP Coursework Template", MB_ICONEXCLAMATION | MB_SYSTEMMODAL);
			this->RequestTermination();
			return(false);
		}
		// If using the reference device, utilize a fixed time step for any animations.
		this->m_pTimer->SetFixedTimeStep(1.0f / 10.0f);
	}

	// Create a swap chain for the window that we started out with.  This
	// demonstrates using a configuration object for fast and concise object
	// creation.
	SwapChainConfigDX11 tconfig;
	tconfig.SetWidth(this->m_pWindow->GetWidth());
	tconfig.SetHeight(this->m_pWindow->GetHeight());
	tconfig.SetOutputWindow(this->m_pWindow->GetHandle());
	this->m_iSwapChain = this->m_pRenderer11->CreateSwapChain(&tconfig);
	this->m_pWindow->SetSwapChain(this->m_iSwapChain);

	//Create Colour and Depth Buffers
	this->m_RenderTarget = this->m_pRenderer11->GetSwapChainResource(this->m_iSwapChain);

	Texture2dConfigDX11 tdepthconfig;
	tdepthconfig.SetDepthBuffer(m_iscreenWidth, m_iscreenHeight);
	this->m_DepthTarget = this->m_pRenderer11->CreateTexture2D(&tdepthconfig, 0);

	// Bind the swap chain render target and the depth buffer for use in rendering.  
	this->m_pRenderer11->pImmPipeline->ClearRenderTargets();
	this->m_pRenderer11->pImmPipeline->OutputMergerStage.DesiredState.RenderTargetViews.SetState(0, this->m_RenderTarget->m_iResourceRTV);
	this->m_pRenderer11->pImmPipeline->OutputMergerStage.DesiredState.DepthTargetViews.SetState(this->m_DepthTarget->m_iResourceDSV);
	this->m_pRenderer11->pImmPipeline->ApplyRenderTargets();

	D3D11_VIEWPORT tviewport;
	tviewport.Width = static_cast<float>(m_iscreenWidth);
	tviewport.Height = static_cast<float>(m_iscreenHeight);
	tviewport.MinDepth = 0.0f;
	tviewport.MaxDepth = 1.0f;
	tviewport.TopLeftX = 0;
	tviewport.TopLeftY = 0;

	int tvpindex = this->m_pRenderer11->CreateViewPort(tviewport);
	this->m_pRenderer11->pImmPipeline->RasterizerStage.DesiredState.ViewportCount.SetState(1);
	this->m_pRenderer11->pImmPipeline->RasterizerStage.DesiredState.Viewports.SetState(0, tvpindex);
	return(true);
}

//////////////////////////////////
//Handle Input Events in the Application
//////////////////////////////////
//////////////////////////////////
//Handle Input Events in the Application
//////////////////////////////////
bool LJMULevelDemo::HandleEvent(EventPtr pevent)
{
	eEVENT e = pevent->GetEventType();

	if (e == SYSTEM_KEYBOARD_KEYDOWN)
	{
		EvtKeyDownPtr tkey_down = std::static_pointer_cast<EvtKeyDown>(pevent);
		unsigned int tkeycode = tkey_down->GetCharacterCode();

		// Check for key presses to modify car speed
		if (tkeycode == VK_UP) // Virtual-Key Code for Up Arrow
		{
			m_carLinearSpeed += 5.0f; // Increase speed
			Log::Get().Write(L"Car speed increased to: " + std::to_wstring(m_carLinearSpeed));
		}
		else if (tkeycode == VK_DOWN) // Virtual-Key Code for Down Arrow
		{
			m_carLinearSpeed -= 5.0f; // Decrease speed
			if (m_carLinearSpeed < 0.0f)
				m_carLinearSpeed = 0.0f; // Prevent negative speed
			Log::Get().Write(L"Car speed decreased to: " + std::to_wstring(m_carLinearSpeed));
		}
	}
	else if (e == SYSTEM_KEYBOARD_KEYUP)
	{
		EvtKeyUpPtr tkey_up = std::static_pointer_cast<EvtKeyUp>(pevent);
		unsigned int tkeycode = tkey_up->GetCharacterCode();
	}

	return(Application::HandleEvent(pevent));
}


//////////////////////////////////
// Destroy Resources created by the engine
//////////////////////////////////
void LJMULevelDemo::ShutdownEngineComponents()
{
	if (this->m_pRenderer11)
	{
		this->m_pRenderer11->Shutdown();
		delete this->m_pRenderer11;
	}

	if (this->m_pWindow)
	{
		this->m_pWindow->Shutdown();
		delete this->m_pWindow;
	}
}

//////////////////////////////////
// Shutdown the Application
//////////////////////////////////
void LJMULevelDemo::Shutdown()
{
	//NOTHING TO DO HERE
}

//////////////////////////////////
// Take a Screenshot of the Application
//////////////////////////////////
void LJMULevelDemo::TakeScreenShot()
{
	if (this->m_bSaveScreenshot)
	{
		this->m_bSaveScreenshot = false;
		this->m_pRenderer11->pImmPipeline->SaveTextureScreenShot(0, this->GetName());
	}
}

//////////////////////////////////////
// Output our Frame Rate
//////////////////////////////////////
std::wstring LJMULevelDemo::outputFPSInfo()
{
	std::wstringstream out;
	out << L"FPS: " << m_pTimer->Framerate();
	return out.str();
}

void LJMULevelDemo::setupLightSources() {

	// Directional Light properties - sun
	Vector3f directionalLightDir = Vector3f(0.0f, -0.5f, 0.5f);
	directionalLightDir.Normalize();
	DirectionalLightDirection = Vector4f(directionalLightDir, 1.0f);
	DirectionalLightColour = Vector4f(0.002, 0.002, 0.002, 1.0f);

	// Point Light properties
	PointLightPosition = Vector4f(0.0f, 30.0f, 0.0f, 1.0f);
	PointLightColour = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
	PointLightRange = Vector4f(20.0f, 0.0f, 0.0f, 0.0f);
	// The range of the point light source is set to 30.0.
	// We only use the first component of the 4D vector because
	// we cannot just send a scalar number to the GPU.

	// Spot Light properties - HeadLights
	Vector3f spotLightDir = Vector3f(2.0f, -1.0f, 2.0f);
	spotLightDir.Normalize();
	SpotLightDirection = Vector4f(spotLightDir, 1.0f);
	SpotLightColour = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
	SpotLightPosition = Vector4f(100.0f, 10.0f, 100.0f, 1.0f);
	SpotLightRange = Vector4f(150.0f, 0.0f, 0.0f, 0.0f);
	SpotLightFocus = Vector4f(20.0f, 0.0f, 0.0f, 0.0f);

	//Second Spotlight
	Vector3f spotLightDir2 = Vector3f(2.0f, -1.0f, 2.0f);
	spotLightDir2.Normalize();
	SpotLightDirection = Vector4f(spotLightDir2, 1.0f);
	SpotLightColour = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
	SpotLightPosition = Vector4f(100.0f, 10.0f, 100.0f, 1.0f);
	SpotLightRange = Vector4f(150.0f, 0.0f, 0.0f, 0.0f);
	SpotLightFocus = Vector4f(20.0f, 0.0f, 0.0f, 0.0f);

}

void LJMULevelDemo::setLights2Material(MaterialPtr material) {
	material->Parameters.SetVectorParameter(L"DirectionalLightColour", DirectionalLightColour);
	material->Parameters.SetVectorParameter(L"DirectionalLightDirection", DirectionalLightDirection);

	material->Parameters.SetVectorParameter(L"SpotLightColour", SpotLightColour);
	material->Parameters.SetVectorParameter(L"SpotLightDirection", SpotLightDirection);
	material->Parameters.SetVectorParameter(L"SpotLightPosition", SpotLightPosition);
	material->Parameters.SetVectorParameter(L"SpotLightRange", SpotLightRange);
	material->Parameters.SetVectorParameter(L"SpotLightFocus", SpotLightFocus);

	material->Parameters.SetVectorParameter(L"PointLightColour", PointLightColour);
	material->Parameters.SetVectorParameter(L"PointLightPosition", PointLightPosition);
	material->Parameters.SetVectorParameter(L"PointLightRange", PointLightRange);
}

MaterialPtr LJMULevelDemo::setupMaterialProperties(MaterialPtr material) {
	float ambient_constant = 0.001f;
	float diffuse_constant = 4.0f;
	float specular_constant = 0.1f;
	float shininess_constant = 200.0f;
	Vector4f emmisivity = Vector4f(0.0f, 0.0f, 0.0f, 1.0f);

	material->Parameters.SetVectorParameter(L"SurfaceConstants", Vector4f(
		ambient_constant,
		diffuse_constant,
		specular_constant,
		shininess_constant));

	material->Parameters.SetVectorParameter(L"SurfaceEmmisiveColour", emmisivity);
	setLights2Material(material);
	return material;
}

void LJMULevelDemo::updateLightSources()
{
	// Get the car's position and rotation
	Vector3f carPosition = m_carActor->GetNode()->Position();
	Matrix3f carRotation = m_carActor->GetNode()->Rotation();

	// Update the spotlight direction to match the car's forward direction
	m_currentCarDirection = carRotation * m_referenceCarDirection;
	m_currentCarDirection.Normalize();

	// Add a small offset to avoid perfect alignment with an axis
	Vector3f adjustedDirection = m_currentCarDirection + Vector3f(0.1f, -0.1f, 0.0f);
	adjustedDirection.Normalize();
	SpotLightDirection = Vector4f(adjustedDirection, 1.0f);

	// Update the spotlight position to match the car's position (left headlight)
	SpotLightPosition = Vector4f(carPosition.x - 1.0f, carPosition.y + 1.0f, carPosition.z, 1.0f);

	// Ensure light color and intensity are maintained
	SpotLightColour = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
	SpotLightFocus = Vector4f(60.0f, 0.0f, 0.0f, 0.0f);  // Increase focus for wider spread
	SpotLightRange = Vector4f(200.0f, 0.0f, 0.0f, 0.0f);  // Increase range for better visibility

	// Add a second spotlight to mimic the right headlight
	Vector3f adjustedDirection2 = m_currentCarDirection + Vector3f(0.1f, -0.1f, 0.0f);
	adjustedDirection2.Normalize();
	SpotLightDirection2 = Vector4f(adjustedDirection2, 1.0f);

	// Update the second spotlight position to match the car's position (right headlight)
	SpotLightPosition2 = Vector4f(carPosition.x + 1.0f, carPosition.y + 1.0f, carPosition.z + 5.0f, 1.0f);

	// Ensure the second light color and intensity are maintained
	SpotLightColour2 = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
	SpotLightFocus2 = Vector4f(60.0f, 0.0f, 0.0f, 0.0f);  // Increase focus for wider spread
	SpotLightRange2 = Vector4f(200.0f, 0.0f, 0.0f, 0.0f);  // Increase range for better visibility
}

void LJMULevelDemo::applyLights2AllMaterials()
{
	MaterialPtr material = m_platformActor->GetBody()->GetMaterial();

	setLights2Material(material);

	m_platformActor->GetBody()->SetMaterial(material);

	material = m_carActor->GetBody()->GetMaterial();
	setLights2Material(material);
	m_carActor->GetBody()->SetMaterial(material);
}

BasicMeshPtr LJMULevelDemo::generateOBJMesh(std::wstring pmeshname, Vector4f pmeshcolour) {
	FileSystem fs;
	LJMUMeshOBJ* tmesh = new LJMUMeshOBJ(fs.GetModelsFolder() + pmeshname);
	int tvertcount = tmesh->positions.size();

	auto tia = std::make_shared < DrawExecutorDX11 < BasicVertexDX11::Vertex>>();
	tia->SetLayoutElements(BasicVertexDX11::GetElementCount(), BasicVertexDX11::Elements);
	tia->SetPrimitiveType(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	tia->SetMaxVertexCount(tvertcount);

	BasicVertexDX11::Vertex tv;
	tv.color = pmeshcolour;

	for (auto& tobject : tmesh->objects) {
		for (auto& tface : tobject.faces) {
			for (size_t i = 0; i < 3; ++i) {
				tv.position = tmesh->positions[tface.PositionIndices[i]];
				tv.normal = tmesh->normals[tface.NormalIndices[i]];
				tv.texcoords = tmesh->coords[tface.CoordIndices[i]];
				tia->AddVertex(tv);
			}
		}
	}
	return tia;

}

MaterialPtr LJMULevelDemo::createLitTexturedMaterial()
{
	RendererDX11* pRenderer = RendererDX11::Get();
	MaterialPtr material = MaterialPtr(new MaterialDX11);

	//Create and fill the effect that will be used for this view type
	RenderEffectDX11* pEffect = new RenderEffectDX11();

	pEffect->SetVertexShader(pRenderer->LoadShader(VERTEX_SHADER,
		std::wstring(L"LJMULitTexture.hlsl"),
		std::wstring(L"VSMAIN"),
		std::wstring(L"vs_4_0")));

	pEffect->SetPixelShader(pRenderer->LoadShader(PIXEL_SHADER,
		std::wstring(L"LJMULitTexture.hlsl"),
		std::wstring(L"PSMAIN"),
		std::wstring(L"ps_4_0")));

	SamplerStateConfigDX11 SamplerConfig;
	SamplerConfig.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerConfig.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerConfig.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SamplerConfig.MaxAnisotropy = 16;

	int TextureSampler = RendererDX11::Get()->CreateSamplerState(&SamplerConfig);
	material->Parameters.SetSamplerParameter(L"TextureSampler", TextureSampler);

	//Enable the material to render the given view type, and set its effect
	material->Params[VT_PERSPECTIVE].bRender = true;
	material->Params[VT_PERSPECTIVE].pEffect = pEffect;

	return material;

}

void LJMULevelDemo::applyTexture2Material(MaterialPtr material, ResourcePtr texture)
{
	material->Parameters.SetShaderResourceParameter(L"ColorTexture", texture);
}

