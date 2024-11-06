// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "DXF.h"	// include dxframework
#include "TextureShader.h"
#include "ShadowShader.h"
#include "DepthShader.h"

class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);

	bool frame();

protected:
	bool render();
	void depthPass(int i);
	void finalPass();
	void gui();

private:
	TextureShader* textureShader;
	PlaneMesh* mesh;

	const int lightCount = 2;

	float time = 0.0f;
	float testScale = 1.0f;

	Light** lights;

	AModel* model;
	AModel* model2;

	ShadowShader* shadowShader;
	DepthShader* depthShader;

	ShadowMap** shadowMaps;
	
	XMFLOAT3 shadow1Pos = XMFLOAT3(0.0f, 0.0f, -20.0f);
	XMFLOAT3 shadow1Dir = XMFLOAT3(0.0f, -0.7f, 0.7f);
	XMFLOAT3 shadow2Pos = XMFLOAT3(0.0f, 0.0f, -20.0f);
	XMFLOAT3 shadow2Dir = XMFLOAT3(0.0f, -0.7f, 0.7f);
};

#endif