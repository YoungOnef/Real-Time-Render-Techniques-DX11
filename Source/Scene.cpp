
//
// Scene.cpp
//

#include <stdafx.h>
#include <string.h>
#include <d3d11shader.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <System.h>
#include <DirectXTK\DDSTextureLoader.h>
#include <DirectXTK\WICTextureLoader.h>
#include <CGDClock.h>
#include <Scene.h>
#include "Grid.h"



#include <Effect.h>
#include <VertexStructures.h>
#include <Texture.h>

using namespace std;
using namespace DirectX;
using namespace DirectX::PackedVector;

//
// Methods to handle initialisation, update and rendering of the scene
HRESULT Scene::rebuildViewport(){
	// Binds the render target view and depth/stencil view to the pipeline.
	// Sets up viewport for the main window (wndHandle) 
	// Called at initialisation or in response to window resize
	ID3D11DeviceContext *context = system->getDeviceContext();
	if (!context)
		return E_FAIL;
	// Bind the render target view and depth/stencil view to the pipeline.
	ID3D11RenderTargetView* renderTargetView = system->getBackBufferRTV();
	context->OMSetRenderTargets(1, &renderTargetView, system->getDepthStencil());
	// Setup viewport for the main window (wndHandle)
	RECT clientRect;
	GetClientRect(wndHandle, &clientRect);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<FLOAT>(clientRect.right - clientRect.left);
	viewport.Height = static_cast<FLOAT>(clientRect.bottom - clientRect.top);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	//Set Viewport
	context->RSSetViewports(1, &viewport);
	return S_OK;
}

// Main resource setup for the application.  These are setup around a given Direct3D device.
HRESULT Scene::initialiseSceneResources() {
	ID3D11DeviceContext *context = system->getDeviceContext();
	ID3D11Device *device = system->getDevice();
	if (!device)
		return E_FAIL;
	// Set up viewport for the main window (wndHandle) 
	rebuildViewport();

	blurUtility = new BlurUtility(device, context);

	//initializes different GPU effects
	//The effects include basic color, basic lighting, per-pixel lighting, reflection mapping, skybox, emissive (sword glow), normal mapping, grass, tree, ocean, and fire effects. 
	//The code specifies the location of the compiled shader files and the vertex descriptions for each effect.
	basicColourEffect = new Effect(device, "Shaders\\cso\\basic_colour_vs.cso", "Shaders\\cso\\basic_colour_ps.cso", basicVertexDesc, ARRAYSIZE(basicVertexDesc));
	basicLightingEffect = new Effect(device, "Shaders\\cso\\basic_lighting_vs.cso", "Shaders\\cso\\basic_colour_ps.cso", extVertexDesc, ARRAYSIZE(extVertexDesc));
	perPixelLightingEffect = new Effect(device, "Shaders\\cso\\per_pixel_lighting_vs.cso", "Shaders\\cso\\per_pixel_lighting_ps.cso", extVertexDesc, ARRAYSIZE(extVertexDesc));
	reflectionMappingEffect = new Effect(device, "Shaders\\cso\\reflection_map_vs.cso", "Shaders\\cso\\reflection_map_ps.cso", extVertexDesc, ARRAYSIZE(extVertexDesc));
	Effect* skyboxEffect = new Effect(device, "Shaders\\cso\\sky_box_vs.cso", "Shaders\\cso\\sky_box_ps.cso", extVertexDesc, ARRAYSIZE(extVertexDesc));
	Effect* perPixelLightingEffect = new Effect(device, "Shaders\\cso\\per_pixel_lighting_vs.cso", "Shaders\\cso\\per_pixel_lighting_ps.cso", extVertexDesc, ARRAYSIZE(extVertexDesc));
	Effect* swordGlowEffect = new Effect(device, "Shaders\\cso\\per_pixel_lighting_vs.cso", "Shaders\\cso\\emissive_ps.cso", extVertexDesc, ARRAYSIZE(extVertexDesc));
	Effect* normalMapEffect = new Effect(device, "Shaders\\cso\\normal_map_ppl_vs.cso", "Shaders\\cso\\normal_map_ppl_ps.cso", extVertexDesc, ARRAYSIZE(extVertexDesc));
	grassEffect = new Effect(device, "Shaders\\cso\\grass_vs.cso", "Shaders\\cso\\grass_ps.cso", extVertexDesc, ARRAYSIZE(extVertexDesc));
	treeEffect = new Effect(device, "Shaders\\cso\\tree_vs.cso", "Shaders\\cso\\tree_ps.cso", extVertexDesc, ARRAYSIZE(extVertexDesc));
	oceanEffect = new Effect(device, "Shaders\\cso\\ocean_vs.cso", "Shaders\\cso\\ocean_ps.cso", extVertexDesc, ARRAYSIZE(extVertexDesc));
	Effect* fireEffect = new Effect(device, "Shaders\\cso\\fire_vs.cso", "Shaders\\cso\\fire_ps.cso", particleVertexDesc, ARRAYSIZE(particleVertexDesc));


	//loads diffenret texture files from disk and creates corresponding Texture objects that can be used in rendering scene
	Texture* cubeDayTexture = new Texture(device, L"Resources\\Textures\\grassenvmap1024.dds");
	Texture* castleTexture = new Texture(device, L"Resources\\Textures\\castle.jpg");
	Texture* castleNormalTexture = new Texture(device, L"Resources\\Textures\\castle_normal.jpg");
	// WoodenLog textures, 3model,, free to use
	//https://www.turbosquid.com/3d-models/wooden-log-8k-3d-1952830
	Texture* rocksDiffuseTexture = new Texture(device, L"Resources\\Textures\\WoodenLog_Diffuse_8K.jpg");
	Texture* rocksNormalTexture = new Texture(device, L"Resources\\Textures\\WoodenLog_normal_8K.jpg");
	Texture* grassAlpha = new Texture(device, L"Resources\\Textures\\grassAlpha.tif");
	Texture* grassTexture = new Texture(device, L"Resources\\Textures\\grass.png");
	Texture* heightMap = new Texture(device, L"Resources\\Textures\\heightmap2.bmp");
	Texture* normalMap = new Texture(device, L"Resources\\Textures\\normalmap.bmp");
	Texture* waterNormalMap = new Texture(device, L"Resources\\Textures\\waves.dds");
	Texture* treeTexture = new Texture(device, L"Resources\\Textures\\tree.tif");
	Texture* logsTexture = new Texture(device, L"Resources\\Textures\\logs.jpg");
	Texture* logsNormalTexture = new Texture(device, L"Resources\\Textures\\logs_normal.jpg");
	Texture* fireTexture = new Texture(device, L"Resources\\Textures\\Fire.tif");
	Texture* swordTexture = new Texture(device, L"Resources\\Textures\\Sword_glow.jpg");


	
	//creates arrays of shader resource views for different textures, each containing the shader resource view for one or more textures. 
	ID3D11ShaderResourceView* castleTextureArray[] = { castleTexture->getShaderResourceView(),castleNormalTexture->getShaderResourceView() };
	ID3D11ShaderResourceView* skyboxTextureArray[] = { cubeDayTexture->getShaderResourceView() };
	ID3D11ShaderResourceView* rocksTextureArray[] = { rocksDiffuseTexture->getShaderResourceView(), rocksNormalTexture->getShaderResourceView() };
	ID3D11ShaderResourceView* logsTextureArray[] = { logsTexture->getShaderResourceView() , logsNormalTexture->getShaderResourceView() };
	ID3D11ShaderResourceView* swordTextureArray[] = { swordTexture->getShaderResourceView() };
	ID3D11ShaderResourceView* grassTextureArray[] = { grassTexture->getShaderResourceView(), grassAlpha->getShaderResourceView() };
	ID3D11ShaderResourceView* waterTextureArray[] = { waterNormalMap->getShaderResourceView(), cubeDayTexture->getShaderResourceView() };
	ID3D11ShaderResourceView* treeTextureArray[] = { treeTexture->getShaderResourceView() };
	
	//modifies the depth stencil state for a skybox effect by getting the current depth stencil state
	ID3D11DepthStencilState* skyboxDSState = skyboxEffect->getDepthStencilState();
	D3D11_DEPTH_STENCIL_DESC skyboxDSDesc;
	skyboxDSState->GetDesc(&skyboxDSDesc);
	skyboxDSDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	skyboxDSState->Release(); device->CreateDepthStencilState(&skyboxDSDesc, &skyboxDSState);
	skyboxEffect->setDepthStencilState(skyboxDSState);

	//retrieves the blend and depth stencil states from a grass effect object and modifies their properties. 
	//It sets up a new blend state to enable transparency and then sets that new blend state to be used by three different effects
	// and  sets up a new depth stencil state for the grass effect and sets that new state to be used by the grass effect
	ID3D11BlendState* transparencyBlendState = grassEffect->getBlendState();
	D3D11_BLEND_DESC transparencyBlendDesc;
	ZeroMemory(&transparencyBlendDesc, sizeof(D3D11_BLEND_DESC));
	transparencyBlendState->GetDesc(&transparencyBlendDesc);
	transparencyBlendDesc.AlphaToCoverageEnable = TRUE;
	transparencyBlendDesc.IndependentBlendEnable = FALSE;
	transparencyBlendDesc.RenderTarget[0].BlendEnable = TRUE;
	transparencyBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	transparencyBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	transparencyBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	transparencyBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	transparencyBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	transparencyBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	transparencyBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	transparencyBlendState->Release(); device->CreateBlendState(&transparencyBlendDesc, &transparencyBlendState);
	grassEffect->setBlendState(transparencyBlendState);
	treeEffect->setBlendState(transparencyBlendState);
	oceanEffect->setBlendState(transparencyBlendState);
	ID3D11DepthStencilState* grassDSstate = grassEffect->getDepthStencilState();
	D3D11_DEPTH_STENCIL_DESC	dsDesc;
	grassDSstate->GetDesc(&dsDesc);
	grassDSstate->Release(); device->CreateDepthStencilState(&dsDesc, &grassDSstate);
	grassEffect->setDepthStencilState(grassDSstate);


	//setting up a blend state for the sword glow effect. It creates a new blend state object and sets up diffnereent properties for it
	D3D11_BLEND_DESC swordGlowBlendDesc;
	ID3D11BlendState* swordGlowBlendState = swordGlowEffect->getBlendState();
	swordGlowBlendState->GetDesc(&swordGlowBlendDesc);
	swordGlowBlendDesc.AlphaToCoverageEnable = FALSE;
	swordGlowBlendDesc.RenderTarget[0].BlendEnable = TRUE;
	swordGlowBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	swordGlowBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	swordGlowBlendState->Release(); device->CreateBlendState(&swordGlowBlendDesc, &swordGlowBlendState);
	swordGlowEffect->setBlendState(swordGlowBlendState);


	//setting up a blend state for the fire effect. It creates a new blend state object and sets up diffnereent properties for it
	ID3D11BlendState* fireBlendState = fireEffect->getBlendState();
	D3D11_BLEND_DESC fireBlendDesc;
	fireBlendState->GetDesc(&fireBlendDesc);
	fireBlendDesc.AlphaToCoverageEnable = FALSE;
	fireBlendDesc.RenderTarget[0].BlendEnable = TRUE;
	fireBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	fireBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	fireBlendState->Release(); device->CreateBlendState(&fireBlendDesc, &fireBlendState);
	fireEffect->setBlendState(fireBlendState);
	ID3D11DepthStencilState* fireDSstate = fireEffect->getDepthStencilState();
	D3D11_DEPTH_STENCIL_DESC fireDsDesc;
	fireDSstate->GetDesc(&fireDsDesc);
	fireDsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	fireDSstate->Release(); device->CreateDepthStencilState(&fireDsDesc, &fireDSstate);
	fireEffect->setDepthStencilState(fireDSstate);
	ID3D11ShaderResourceView* fireTextureArray[] = { fireTexture->getShaderResourceView() };

	//defines two different Material objects
	Material glossRed(XMCOLOR(1.0, 0.0, 0.0, 1.0));
	Material* glossRedMaterialArray[]{ &glossRed };
	Material matWhite;
	matWhite.setSpecular(XMCOLOR(0.2, 0.2, 0.2, 0.01));
	Material* matWhiteArray[]{ &matWhite };

	//initializes  diffneret 3d models
	//Each model is assigned a world matrix
	//Different effects, textures, and materials are also used for each model
	box = new Box(device, skyboxEffect, NULL, 0, skyboxTextureArray, 1);
	box->setWorldMatrix(box->getWorldMatrix() * XMMatrixScaling(2000, 2000, 2000));
	box->update(context);

	castle = new Model(device, wstring(L"Resources\\Models\\castle.3DS"), perPixelLightingEffect, NULL, 0, castleTextureArray, 1);
	castle->setWorldMatrix(castle->getWorldMatrix()* XMMatrixTranslation(0, 0.5, 0)* XMMatrixScaling(2.5, 2.5, 2.5));
	castle->update(context);

	WoodenLog = new Model(device, wstring(L"Resources\\Models\\WoodenLog_obj.OBJ"), perPixelLightingEffect, NULL, 0, rocksTextureArray, 2);
	WoodenLog->setWorldMatrix(WoodenLog->getWorldMatrix() * XMMatrixScaling(0.5, 0.5, 0.5) * XMMatrixTranslation(0, 1.1, 6.0));
	WoodenLog->update(context);

	logs = new  Model(device, wstring(L"Resources\\Models\\logs.obj"), normalMapEffect, matWhiteArray, 1, logsTextureArray, 2);
	logs->setWorldMatrix(logs->getWorldMatrix() * XMMatrixScaling(0.001, 0.001, 0.001) * XMMatrixRotationX(XMConvertToRadians(-80)) * XMMatrixTranslation(0, 1.30f, 11));
	logs->update(context);

	//https://www.turbosquid.com/FullPreview/1283673
	sword = new Model(device, wstring(L"Resources\\Models\\sword.3ds"), swordGlowEffect, NULL, 0, swordTextureArray, 1);
	sword->setWorldMatrix(sword->getWorldMatrix() * XMMatrixScaling(0.0002, 0.0002, 0.0002) * XMMatrixRotationX(XMConvertToRadians(90)) * XMMatrixRotationY(XMConvertToRadians(180)) * XMMatrixRotationZ(XMConvertToRadians(90)) * XMMatrixTranslation(0, 1.6, 6));
	sword->update(context);

	shark = new Model(device, wstring(L"Resources\\Models\\Shark.obj"), reflectionMappingEffect, NULL, 0, skyboxTextureArray, 1);
	shark->setWorldMatrix(XMMatrixScaling(0.5, 0.5, 0.5) * XMMatrixTranslation(15, -1, 0));
	shark->update(context);

	grass = new Terrain(device, context, 128, 128, heightMap->getTexture(), normalMap->getTexture(), grassEffect, NULL, 0, grassTextureArray, 2);
	grass->setWorldMatrix(grass->getWorldMatrix()* XMMatrixTranslation(-52, -0.1, -64)* XMMatrixScaling(1, 3, 1));
	grass->update(context);

	water = new Grid(64, 64, device, oceanEffect, NULL, 0, waterTextureArray, 2);
	water->setWorldMatrix(water->getWorldMatrix()* XMMatrixTranslation(-26, 0, -32));
	water->update(context);

	tree = new Model(device, wstring(L"Resources\\Models\\tree.3ds"), treeEffect, NULL, 0, treeTextureArray, 1);
	tree->setWorldMatrix(tree->getWorldMatrix()* XMMatrixTranslation(-2.5, 0.2, 0)* XMMatrixScaling(1.8, 3, 1.8));
	tree->update(context);

	tree2 = new Model(device, wstring(L"Resources\\Models\\tree.3ds"), treeEffect, NULL, 0, treeTextureArray, 1);
	tree2->setWorldMatrix(tree2->getWorldMatrix()* XMMatrixTranslation(2.5, 0.2, 0)* XMMatrixScaling(1.8, 3, 1.8));
	tree2->update(context);

	fire = new ParticleSystem(device, fireEffect, NULL, 0, fireTextureArray, 1);
	fire->setWorldMatrix(fire->getWorldMatrix()* XMMatrixScaling(1, 1, 1)* XMMatrixTranslation(0, 1.90f, 11));
	fire->update(context);

	//setting up camera for the scene
	mainCamera = new FirstPersonCamera(device, XMVectorSet(-19.0, 2.0, 27.0,1.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f), XMVectorSet(0.8f, 0.0f, -1.0f, 1.0f));mainCamera->setFlying(false);

	cBufferLightCPU = (CBufferLight *)_aligned_malloc(sizeof(CBufferLight), 16);

	// Fill out cBufferLightCPU
	cBufferLightCPU->lightVec = XMFLOAT4(-5.0, 2.0, 5.0, 1.0);
	cBufferLightCPU->lightAmbient = XMFLOAT4(0.2, 0.2, 0.2, 1.0);
	cBufferLightCPU->lightDiffuse = XMFLOAT4(0.7, 0.7, 0.7, 1.0);
	cBufferLightCPU->lightSpecular = XMFLOAT4(1.0, 1.0, 1.0, 1.0);

	// Create GPU resource memory copy of cBufferLight
	// fill out description (Note if we want to update the CBuffer we need  D3D11_CPU_ACCESS_WRITE)
	D3D11_BUFFER_DESC cbufferDesc;
	D3D11_SUBRESOURCE_DATA cbufferInitData;
	ZeroMemory(&cbufferDesc, sizeof(D3D11_BUFFER_DESC));
	ZeroMemory(&cbufferInitData, sizeof(D3D11_SUBRESOURCE_DATA));

	cbufferDesc.ByteWidth = sizeof(CBufferLight);
	cbufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbufferInitData.pSysMem = cBufferLightCPU;// Initialise GPU CBuffer with data from CPU CBuffer

	HRESULT hr = device->CreateBuffer(&cbufferDesc, &cbufferInitData,
		&cBufferLightGPU);

	// We dont strictly need to call map here as the GPU CBuffer was initialised from the CPU CBuffer at creation.
	// However if changes are made to the CPU CBuffer during update the we need to copy the data to the GPU CBuffer 
	// using the mapCbuffer helper function provided the in Util.h   

	mapCbuffer(context, cBufferLightCPU, cBufferLightGPU, sizeof(CBufferLight));
	context->VSSetConstantBuffers(2, 1, &cBufferLightGPU);// Attach CBufferLightGPU to register b2 for the vertex shader. Not strictly needed as our vertex shader doesnt require access to this CBuffer
	context->PSSetConstantBuffers(2, 1, &cBufferLightGPU);// Attach CBufferLightGPU to register b2 for the Pixel shader.

	// Add a Cbuffer to stor world/scene properties
	// Allocate 16 byte aligned block of memory for "main memory" copy of cBufferLight
	cBufferSceneCPU = (CBufferScene *)_aligned_malloc(sizeof(CBufferScene), 16);

	// Fill out cBufferSceneCPU
	cBufferSceneCPU->windDir = XMFLOAT4(1, 0, 0, 1);
	cBufferSceneCPU->Time = 0.0;
	cBufferSceneCPU->grassHeight = 0.0;
	
	cbufferInitData.pSysMem = cBufferSceneCPU;// Initialise GPU CBuffer with data from CPU CBuffer
	cbufferDesc.ByteWidth = sizeof(CBufferScene);

	hr = device->CreateBuffer(&cbufferDesc, &cbufferInitData, &cBufferSceneGPU);

	mapCbuffer(context, cBufferSceneCPU, cBufferSceneGPU, sizeof(CBufferScene));
	context->VSSetConstantBuffers(3, 1, &cBufferSceneGPU);// Attach CBufferSceneGPU to register b3 for the vertex shader. Not strictly needed as our vertex shader doesnt require access to this CBuffer
	context->PSSetConstantBuffers(3, 1, &cBufferSceneGPU);// Attach CBufferSceneGPU to register b3 for the Pixel shader

	return S_OK;
}

// Update scene state (perform animations etc)
HRESULT Scene::updateScene(ID3D11DeviceContext *context,Camera *camera) {

	// mainClock is a helper class to manage game time data
	mainClock->tick();
	double dT = mainClock->gameTimeDelta();
	double gT = mainClock->gameTimeElapsed();
	//cout << "Game time Elapsed= " << gT << " seconds" << endl;

	// If the CPU CBuffer contents are changed then the changes need to be copied to GPU CBuffer with the mapCbuffer helper function
	mainCamera->update(context);

	// Update the scene time as it is needed to animate the water
	cBufferSceneCPU->Time = gT;
	mapCbuffer(context, cBufferSceneCPU, cBufferSceneGPU, sizeof(CBufferScene));
	cout << "AVG SPF - " << mainClock->averageSPF() << endl;
	cout << "AVG FPS - " << mainClock->averageFPS() << endl; 
	
	return S_OK;
}

// Render scene
HRESULT Scene::renderScene() {

	ID3D11DeviceContext *context = system->getDeviceContext();
	
	// Validate window and D3D context
	if (isMinimised() || !context)
		return E_FAIL;
	
	// Clear the screen
	static const FLOAT clearColor[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
	context->ClearRenderTargetView(system->getBackBufferRTV(), clearColor);
	context->ClearDepthStencilView(system->getDepthStencil(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	
	// Render SkyBox
	if (box)
		box->render(context);

	// Render Orb
	if (water)
		water->render(context);
	if(shark)
		shark->render(context);

	if (grass) {

		// Render grass layers from base to tip
		for (int i = 0; i < numGrassPasses; i++)
		{
			cBufferSceneCPU->grassHeight = (grassLength / numGrassPasses) * i;
			mapCbuffer(context, cBufferSceneCPU, cBufferSceneGPU, sizeof(CBufferScene));
			grass->render(context);
		}
	}

	if(tree)
		tree->render(context);
	if (tree2)
		tree2->render(context);

	if (castle)
		castle->render(context);

	if (WoodenLog)
		WoodenLog->render(context);

	if (sword)
	{
		blurUtility->blurModel(sword, system->getDepthStencilSRV());
		sword->render(context);
	}
	if (logs)
		logs->render(context);

	if (fire)
		fire->render(context);


	// Present current frame to the screen
	HRESULT hr = system->presentBackBuffer();

	return S_OK;
}

//
// Event handling methods
//
// Process mouse move with the left button held down
void Scene::handleMouseLDrag(const POINT &disp) {
	//LookAtCamera

	//mainCamera->rotateElevation((float)-disp.y * 0.01f);
	//mainCamera->rotateOnYAxis((float)-disp.x * 0.01f);

	//FirstPersonCamera
		mainCamera->elevate((float)-disp.y * 0.01f);
		mainCamera->turn((float)-disp.x * 0.01f);
}

// Process mouse wheel movement
void Scene::handleMouseWheel(const short zDelta) {
	//LookAtCamera

	//if (zDelta<0)
	//	mainCamera->zoomCamera(1.2f);
	//else if (zDelta>0)
	//	mainCamera->zoomCamera(0.9f);
	//cout << "zoom" << endl;
	//FirstPersonCamera
	mainCamera->move(zDelta*0.01);
}

// Process key down event.  keyCode indicates the key pressed while extKeyFlags indicates the extended key status at the time of the key down event (see http://msdn.microsoft.com/en-gb/library/windows/desktop/ms646280%28v=vs.85%29.aspx).
void Scene::handleKeyDown(const WPARAM keyCode, const LPARAM extKeyFlags) {
	// Add key down handler here...
		if (keyCode == VK_HOME)
		mainCamera->elevate(0.05f);

	if (keyCode == VK_END)
		mainCamera->elevate(-0.05f);

	if (keyCode == VK_LEFT)
		mainCamera->turn(-0.05f);

	if (keyCode == VK_RIGHT)
		mainCamera->turn(0.05f);

	if (keyCode == VK_SPACE)
	{
		bool isFlying = mainCamera->toggleFlying();
		if (isFlying)
			cout << "Flying mode is on" << endl;
		else
			cout << "Flying mode is off" << endl;
	}
	if (keyCode == VK_UP)
		mainCamera->move(0.5);

	if (keyCode == VK_DOWN)
		mainCamera->move(-0.5);

}

// Process key up event.  keyCode indicates the key released while extKeyFlags indicates the extended key status at the time of the key up event (see http://msdn.microsoft.com/en-us/library/windows/desktop/ms646281%28v=vs.85%29.aspx).
void Scene::handleKeyUp(const WPARAM keyCode, const LPARAM extKeyFlags) {
	// Add key up handler here...
}






// Clock handling methods
void Scene::startClock() {
	mainClock->start();
}

void Scene::stopClock() {
	mainClock->stop();
}

void Scene::reportTimingData() {

	cout << "Actual time elapsed = " << mainClock->actualTimeElapsed() << endl;
	cout << "Game time elapsed = " << mainClock->gameTimeElapsed() << endl << endl;
	mainClock->reportTimingData();
}

// Private constructor
Scene::Scene(const LONG _width, const LONG _height, const wchar_t* wndClassName, const wchar_t* wndTitle, int nCmdShow, HINSTANCE hInstance, WNDPROC WndProc) {
	try
	{
		// 1. Register window class for main DirectX window
		WNDCLASSEX wcex;
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInstance;
		wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wcex.hCursor = LoadCursor(NULL, IDC_CROSS);
		wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wcex.lpszMenuName = NULL;
		wcex.lpszClassName = wndClassName;
		wcex.hIconSm = NULL;
		if (!RegisterClassEx(&wcex))
			throw exception("Cannot register window class for Scene HWND");
		// 2. Store instance handle in our global variable
		hInst = hInstance;
		// 3. Setup window rect and resize according to set styles
		RECT		windowRect;
		windowRect.left = 0;
		windowRect.right = _width;
		windowRect.top = 0;
		windowRect.bottom = _height;
		DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		DWORD dwStyle = WS_OVERLAPPEDWINDOW;
		AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);
		// 4. Create and validate the main window handle
		wndHandle = CreateWindowEx(dwExStyle, wndClassName, wndTitle, dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 500, 500, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, NULL, NULL, hInst, this);
		if (!wndHandle)
			throw exception("Cannot create main window handle");
		ShowWindow(wndHandle, nCmdShow);
		UpdateWindow(wndHandle);
		SetFocus(wndHandle);
		// 5. Create DirectX host environment (associated with main application wnd)
		system = System::CreateDirectXSystem(wndHandle);
		if (!system)
			throw exception("Cannot create Direct3D device and context model");
		// 6. Setup application-specific objects
		HRESULT hr = initialiseSceneResources();
		if (!SUCCEEDED(hr))
			throw exception("Cannot initalise scene resources");
		// 7. Create main clock / FPS timer (do this last with deferred start of 3 seconds so min FPS / SPF are not skewed by start-up events firing and taking CPU cycles).
		mainClock = CGDClock::CreateClock(string("mainClock"), 3.0f);
		if (!mainClock)
			throw exception("Cannot create main clock / timer");
	}
	catch (exception &e)
	{
		cout << e.what() << endl;
		// Re-throw exception
		throw;
	}
}

// Helper function to call updateScene followed by renderScene
HRESULT Scene::updateAndRenderScene() {
	ID3D11DeviceContext *context = system->getDeviceContext();
	HRESULT hr = updateScene(context, (Camera*)mainCamera);
	if (SUCCEEDED(hr))
		hr = renderScene();
	return hr;
}

// Return TRUE if the window is in a minimised state, FALSE otherwise
BOOL Scene::isMinimised() {

	WINDOWPLACEMENT				wp;

	ZeroMemory(&wp, sizeof(WINDOWPLACEMENT));
	wp.length = sizeof(WINDOWPLACEMENT);

	return (GetWindowPlacement(wndHandle, &wp) != 0 && wp.showCmd == SW_SHOWMINIMIZED);
}

//
// Public interface implementation
//
// Method to create the main Scene instance
Scene* Scene::CreateScene(const LONG _width, const LONG _height, const wchar_t* wndClassName, const wchar_t* wndTitle, int nCmdShow, HINSTANCE hInstance, WNDPROC WndProc) {
	static bool _scene_created = false;
	Scene *scene = nullptr;
	if (!_scene_created) {
		scene = new Scene(_width, _height, wndClassName, wndTitle, nCmdShow, hInstance, WndProc);
		if (scene)
			_scene_created = true;
	}
	return scene;
}

// Destructor
Scene::~Scene() {
	
	//Clean Up

	if (cBufferSceneCPU)
		_aligned_free(cBufferSceneCPU);
	if (cBufferLightCPU)
		_aligned_free(cBufferLightCPU);
	if (cBufferSceneGPU)
		cBufferSceneGPU->Release();
	if (cBufferLightGPU)
		cBufferLightGPU->Release();

	
	// Delete Scene Textures
	if (cubeDayTexture)
		delete(cubeDayTexture);
	if (brickTexture)
		delete(brickTexture);

	// Delete Scene Effects
	if (basicColourEffect)
		delete(basicColourEffect);
	if (basicLightingEffect)
		delete(basicLightingEffect);
	if (perPixelLightingEffect)
		delete(perPixelLightingEffect);
	if (skyBoxEffect)
		delete(skyBoxEffect);
	if (reflectionMappingEffect)
		delete(reflectionMappingEffect);


	// Delete Scene objects

	if (box)
		delete(box);
	if (orb)
		delete(orb);
	if (orb2)
		delete(orb2);

	if (mainClock)
		delete(mainClock);
	if (mainCamera)
		delete(mainCamera);
	
	if (system)
		delete(system);

	if (wndHandle)
		DestroyWindow(wndHandle);
	//done
}

// Call DestoryWindow on the HWND
void Scene::destoryWindow() {
	if (wndHandle != NULL) {
		HWND hWnd = wndHandle;
		wndHandle = NULL;
		DestroyWindow(hWnd);
	}
}

//
// Private interface implementation
//
// Resize swap chain buffers and update pipeline viewport configurations in response to a window resize event
HRESULT Scene::resizeResources() {
	if (system) {
		// Only process resize if the System *system exists (on initial resize window creation this will not be the case so this branch is ignored)
		HRESULT hr = system->resizeSwapChainBuffers(wndHandle);
		rebuildViewport();
		RECT clientRect;
		GetClientRect(wndHandle, &clientRect);
		if (!isMinimised())
			renderScene();
	}
	return S_OK;
}

