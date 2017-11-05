#pragma once

#include "Window.h"

#include <d3d11.h>

// We can include the correct library files here
// instead of in Visual Studio settings if we want
#pragma comment(lib, "d3d11.lib")

class DXCore
{
public:
	DXCore(HINSTANCE hInstance,
		unsigned int windowWidth,	// Width of the window's client area
		unsigned int windowHeight,	// Height of the window's client area
		char* titleBarText,			// Text for the window's title bar
		bool debugTitleBarStats);	// The application's handle
	~DXCore();

	// Static requirements for OS-level message processing
	static DXCore* DXCoreInstance;
	static LRESULT CALLBACK WindowProc(
		HWND hWnd,		// Window handle
		UINT uMsg,		// Message
		WPARAM wParam,	// Message's first parameter
		LPARAM lParam	// Message's second parameter
		);

	// Internal method for message handling
	virtual LRESULT ProcessMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// Initialization and game-loop related methods
	HRESULT InitDirectX();
	HRESULT Run();				
	static void Quit();
	virtual void OnResize();
	
	// Pure virtual methods for setup and game functionality
	virtual HRESULT Init();
	virtual void Update(float deltaTime, float totalTime)	= 0;
	virtual void Draw(float deltaTime, float totalTime)		= 0;
	
protected:
	Window* m_window;

	// DirectX related objects and variables
	D3D_FEATURE_LEVEL		dxFeatureLevel;
	IDXGISwapChain*			swapChain;
	ID3D11Device*			device;
	ID3D11DeviceContext*	context;

	ID3D11RenderTargetView* backBufferRTV;
	ID3D11DepthStencilView* depthStencilView;

private:
	std::string m_titleBarText;	// Custom text in window's title bar
	bool m_titleBarStats;	// Show extra stats in title bar?

	// Timing related data
	double perfCounterSeconds;
	float totalTime;
	float deltaTime;
	__int64 startTime;
	__int64 currentTime;
	__int64 previousTime;

	// FPS calculation
	int fpsFrameCount;
	float fpsTimeElapsed;
	
	void UpdateTimer();			// Updates the timer for this frame
	void UpdateTitleBarStats();	// Puts debug info in the title bar
};

