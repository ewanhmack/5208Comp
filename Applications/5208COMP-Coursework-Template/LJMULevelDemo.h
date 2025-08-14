#pragma once

#include "Application.h"
#include "LJMUGeometryActor.h"
#include "Win32RenderWindow.h"
#include "RendererDX11.h"

#include "ViewPerspective.h"

//Hieroglyph Includes
#include "Camera.h"
#include "Scene.h"
#include "GeometryActor.h"
#include "PointLight.h"

//STL Includes
#include <vector>

//LJMU Framework Includes
#include "LJMUTextOverlay.h"
#include "LJMUSpriteOverlay.h"

#include <BasicVertexDX11.h>
typedef std::shared_ptr<Glyph3::DrawExecutorDX11<Glyph3::BasicVertexDX11::Vertex>> BasicMeshPtr;

using namespace Glyph3;

namespace LJMUDX
{
	//////////////////////////////////////
	//LJMULevelDemo.H
	//Class Application for a DirectX 11
	//Driven Application using the DirectX Toolkit
	//Hieroglyph 3 Rendering Engine and LUA.
	//
	//
	//AUTHORS:  DR PO YANG
	//			DR CHRIS CARTER
	//////////////////////////////////////

	class LJMULevelDemo : public Application //Inherit from the Hieroglyph Base Class
	{

	public:
		//------------CONSTRUCTORS------------------------------------------------
		LJMULevelDemo();	//Standard Empty Constructor which builds the object

	public:
		//------------INHERITED METHODS-------------------------------------------
		virtual void Initialize();					//Initialise the DirectX11 Scene
		virtual void Update();						//Update the DirectX Scene
		virtual void Shutdown();					//Shutdown the DirectX11 Scene

		virtual bool ConfigureEngineComponents();	//Initialise Hieroglyph and DirectX TK Modules
		virtual void ShutdownEngineComponents();	//Destroy Hieroglyph and DirectX TK Modules

		virtual void TakeScreenShot();				//Allow a screenshot to be generated

		virtual bool HandleEvent(EventPtr pEvent);	//Handle an I/O Event
		virtual std::wstring GetName();				//Get the Name of this App Instance

		//------------CUSTOM METHODS-----------------------------------------------
		void inputAssemblyStage();					//Stage to setup our VB and IB Info

		std::wstring outputFPSInfo();				//Convert the timer's Frames Per Second to a formatted string

	protected:
		//-------------CLASS MEMBERS-----------------------------------------------
		RendererDX11* m_pRenderer11;		//Pointer to our DirectX 11 Device
		Win32RenderWindow* m_pWindow;			//Pointer to our Windows-Based Window

		int						m_iSwapChain;		//Index of our Swap Chain 
		ResourcePtr				m_RenderTarget;		//Pointer to the GPU Render Target for Colour
		ResourcePtr				m_DepthTarget;		//Pointer to the GPU Render Target for Depth

		std::vector<Vector3f> m_checkpoints;

		float m_carLinerarSpeed;
		float m_carAngularSpeed;
		float m_carDistance2NextCheckpoint;
		float m_carLinearSpeed = 40.0f;

		int m_currentCheckpointIndex;
		int m_nextCheckpointIndex;
		int m_carState;

		Vector3f m_referenceCarDirection;
		Vector3f m_currentCarDirection;
		Vector3f m_targetCarDirection;

		const float DEG_TO_RAD = GLYPH_PI / 180.0f;
		
		//Objects
		LJMUSpriteOverlay* m_pRender_sprite;
		ResourcePtr m_textSprite;


		//Track
		LJMUGeometryActor* m_platformActor;
		ResourcePtr m_racetrackTexture;
		float m_platformWidth = 256.0f;
		float m_platformLength = 256.0f;

		//Car
		LJMUGeometryActor* m_carActor;
		ResourcePtr m_carTexture;

		//Tree
		LJMUGeometryActor* m_treeTrunkActor;
		LJMUGeometryActor* m_treeTopActor;
		ResourcePtr m_treeTopTexture;
		ResourcePtr m_treeStemTexture;
		std::vector<Actor*> m_treeActor;
		Actor* m_treeTrunkPivot;

		//Trex
		ResourcePtr m_trexTexture;
		LJMUGeometryActor* m_trexActor;

		//Reptile
		ResourcePtr m_lizardTexture;
		LJMUGeometryActor* m_lizardActor;

		//bull
		ResourcePtr m_bullTexture;
		LJMUGeometryActor* m_bullActor;



		//Sky and day-night system
		LJMUGeometryActor* m_SkyLine;
		ResourcePtr m_SkylineDayTexture;
		ResourcePtr m_SkylineNightTexture;
		ResourcePtr m_SkyTexture;

		Vector4f m_DayTimeLight = Vector4f(0.07, 0.07, 0.07, 1.0f);
		Vector4f m_NightTimeLight = Vector4f(0.002, 0.002, 0.002, 1.0f);

		void setupLightSources();
		void setLights2Material(MaterialPtr material);
		MaterialPtr setupMaterialProperties(MaterialPtr material);

		// Variables to store light source properties
		// Directional Light Source only has COLOUR and DIRECTION
		Vector4f DirectionalLightColour;
		Vector4f DirectionalLightDirection;

		// Point Light Source has POSITION, COLOUR, and RANGE
		Vector4f PointLightPosition;
		Vector4f PointLightColour;
		Vector4f PointLightRange;

		// Spot Light Source has POSITION, COLOUR, RANGE, DIRECTION and FOCUS
		Vector4f SpotLightPosition;
		Vector4f SpotLightColour;
		Vector4f SpotLightRange;
		Vector4f SpotLightDirection;
		Vector4f SpotLightFocus;

		Vector4f SpotLightPosition2;
		Vector4f SpotLightColour2;
		Vector4f SpotLightRange2;
		Vector4f SpotLightDirection2;
		Vector4f SpotLightFocus2;

		MaterialPtr createLitTexturedMaterial();
		void applyTexture2Material(MaterialPtr material, ResourcePtr texture);
		MaterialPtr CreateInvertedMaterial(ResourcePtr texture);

		//Obj functions
		BasicMeshPtr generateOBJMesh(std::wstring pmeshname, Vector4f pmeshColour);

		float						m_totalPlayTime = 0;

		void			updateLightSources();
		void			applyLights2AllMaterials();

		//--------------HIEROGLYPH OBJECTS-----------------------------------------
		ViewPerspective* m_pRenderView;		//3D Output View - DirectX 11 Accelerated
		LJMUTextOverlay* m_pRender_text;		//2D Output View - DirectX 11 Accelerated
		Camera* m_pCamera;			//Camera Object

		float					m_iscreenWidth = 1920.0f;
		float					m_iscreenHeight = 1080.0f;

		

		
	};

}
