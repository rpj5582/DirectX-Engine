#pragma once

#include <DirectXMath.h>
#include <Windows.h>
#include <string>

class Window
{
public:
	Window(
		HINSTANCE hInstance,		// The application's handle
		unsigned int windowWidth,	// Width of the window's client area
		unsigned int windowHeight);	// Height of the window's client area

	~Window();

	static DirectX::XMFLOAT4X4 getProjectionMatrix();
	static unsigned int getWidth();
	static unsigned int getHeight();

	HRESULT init();
	HWND getWindowHandle();
	void resize(unsigned int width, unsigned int height);
	void setTitleText(std::string text);

private:
	void updateProjectionMatrix(float nearZ, float farZ);

	static Window* m_instance;

	HINSTANCE	m_hInstance;		// The handle to the application
	HWND		m_hWnd;			// The handle to the window itself
	
	// Size of the window's client area
	unsigned int m_width;
	unsigned int m_height;

	DirectX::XMFLOAT4X4 m_projectionMatrix;
};

