// Lab1.cpp
// Lab 1 example, simple coloured triangle mesh
#include "App1.h"

App1::App1()
{

}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in, bool VSYNC, bool FULL_SCREEN)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	// Create Mesh object and shader object
	mesh = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext());
	model = new AModel(renderer->getDevice(), "res/teapot.obj");
	model2 = new AModel(renderer->getDevice(), "res/sphere2.obj");
	textureMgr->loadTexture(L"brick", L"res/brick1.dds");

	// initial shaders
	textureShader = new TextureShader(renderer->getDevice(), hwnd);
	depthShader = new DepthShader(renderer->getDevice(), hwnd);
	shadowShader = new ShadowShader(renderer->getDevice(), hwnd);

	// Variables for defining shadow map
	int shadowmapWidth = 4096;
	int shadowmapHeight = 4096;
	int sceneWidth = 100;
	int sceneHeight = 100;

	shadowMaps = new ShadowMap*[lightCount];
	lights = new Light*[lightCount];

	// This is your shadow map
	for (int i = 0; i < lightCount; i++)
	{
		shadowMaps[i] = new ShadowMap(renderer->getDevice(), shadowmapWidth, shadowmapHeight);

		// Configure directional light
		lights[i] = new Light();
		lights[i]->setAmbientColour(0.3f, 0.3f, 0.3f, 1.0f);
		lights[i]->setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
		lights[i]->generateOrthoMatrix((float)sceneWidth, (float)sceneHeight, 0.1f, 100.f);
	}
}

App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	// Release the Direct3D object.

}


bool App1::frame()
{
	bool result;

	result = BaseApplication::frame();
	if (!result)
	{
		return false;
	}
	
	// Render the graphics.
	result = render();
	if (!result)
	{
		return false;
	}

	return true;
}

bool App1::render()
{
	lights[0]->setDirection(shadow1Dir.x, shadow1Dir.y, shadow1Dir.z);
	lights[0]->setPosition(shadow1Pos.x, shadow1Pos.y, shadow1Pos.z);

	lights[1]->setDirection(shadow2Dir.x, shadow2Dir.y, shadow2Dir.z);
	lights[1]->setPosition(shadow2Pos.x, shadow2Pos.y, shadow2Pos.z);

	// Perform depth pass
	for (int i = 0; i < lightCount; i++)
		depthPass(i);
	// Render scene
	finalPass();

	time += 0.01f;

	return true;
}

void App1::depthPass(int i)
{
	// Set the render target to be the render to texture.
	shadowMaps[i]->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());

	// get the world, view, and projection matrices from the camera and d3d objects.
	lights[i]->generateViewMatrix();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX lightViewMatrix = lights[i]->getViewMatrix();
	XMMATRIX lightProjectionMatrix = lights[i]->getOrthoMatrix();

	worldMatrix = XMMatrixTranslation(-50.f, 0.f, -10.f);
	// Render floor
	mesh->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), mesh->getIndexCount());

	worldMatrix = renderer->getWorldMatrix();
	worldMatrix *= XMMatrixRotationY(time);
	worldMatrix *= XMMatrixTranslation(0.f, 7.f, 5.f);
	XMMATRIX scaleMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	// Render model
	model->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), model->getIndexCount());

	// Render model2
	worldMatrix = renderer->getWorldMatrix();
	scaleMatrix = XMMatrixScaling(testScale, testScale, testScale);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	worldMatrix *= XMMatrixTranslation(10, 0, 0);
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	model2->sendData(renderer->getDeviceContext());
	depthShader->render(renderer->getDeviceContext(), model->getIndexCount());

	// Set back buffer as render target and reset view port.
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}

void App1::finalPass()
{
	// Clear the scene. (default blue colour)
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);
	camera->update();

	// get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	worldMatrix = XMMatrixTranslation(-50.f, 0.f, -10.f);
	// Render floor
	mesh->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, 
		textureMgr->getTexture(L"brick"), shadowMaps, lights, lightCount);
	shadowShader->render(renderer->getDeviceContext(), mesh->getIndexCount());

	// Render model
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix *= XMMatrixRotationY(time);
	worldMatrix *= XMMatrixTranslation(0.f, 7.f, 5.f);
	XMMATRIX scaleMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	model->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"brick"), shadowMaps, lights, lightCount);
	shadowShader->render(renderer->getDeviceContext(), model->getIndexCount());
	
	// Render model2
	worldMatrix = renderer->getWorldMatrix();
	scaleMatrix = XMMatrixScaling(testScale, testScale, testScale);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	worldMatrix *= XMMatrixTranslation(10,0,0);
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"brick"), shadowMaps, lights, lightCount);
	model2->sendData(renderer->getDeviceContext());
	shadowShader->render(renderer->getDeviceContext(), model2->getIndexCount());

	gui();
	renderer->endScene();
}



void App1::gui()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	// Build UI
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);

	ImGui::Text("Shadow 1");
	ImGui::SliderFloat3("Position: ", &shadow1Pos.x, -100.0f, 100.0f);
	ImGui::SliderFloat3("Direction: ", &shadow1Dir.x, -1.0f, 1.0f);

	ImGui::Text("Shadow 2");
	ImGui::SliderFloat3("Position 2: ", &shadow2Pos.x, -100.0f, 100.0f);
	ImGui::SliderFloat3("Direction 2: ", &shadow2Dir.x, -1.0f, 1.0f);

	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

