#include "Window.h"

#include "DXCore.h"
#include "Scene/SceneManager.h"

using namespace DirectX;

Window* Window::m_instance = nullptr;

// --------------------------------------------------------
// Constructor - Set up fields
//
// hInstance	- The application's OS-level handle (unique ID)
// titleBarText - Text for the window's title bar
// windowWidth	- Width of the window's client (internal) area
// windowHeight - Height of the window's client (internal) area
// debugTitleBarStats - Show debug stats in the title bar, like FPS?
// --------------------------------------------------------
Window::Window(HINSTANCE hInstance, unsigned int windowWidth, unsigned int windowHeight)
{
	if (!m_instance) m_instance = this;
	else return;

	m_hInstance = hInstance;
	m_width = windowWidth;
	m_height = windowHeight;

	XMStoreFloat4x4(&m_projectionMatrix, XMMatrixIdentity());
}

Window::~Window()
{
}

// --------------------------------------------------------
// Create the actual window for our application
// --------------------------------------------------------
HRESULT Window::init()
{
	// Start window creation by filling out the
	// appropriate window class struct
	WNDCLASS wndClass = {}; // Zero out the memory
	wndClass.style = CS_HREDRAW | CS_VREDRAW;	// Redraw on horizontal or vertical movement/adjustment
	wndClass.lpfnWndProc = DXCore::WindowProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = m_hInstance;						// Our app's handle
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);	// Default icon
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);		// Default arrow cursor
	wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = "Direct3DWindowClass";

	// Attempt to register the window class we've defined
	if (!RegisterClass(&wndClass))
	{
		// Get the most recent error
		DWORD error = GetLastError();

		// If the class exists, that's actually fine.  Otherwise,
		// we can't proceed with the next step.
		if (error != ERROR_CLASS_ALREADY_EXISTS)
			return HRESULT_FROM_WIN32(error);
	}

	// Adjust the width and height so the "client size" matches
	// the width and height given (the inner-area of the window)
	RECT clientRect;
	SetRect(&clientRect, 0, 0, m_width, m_height);
	AdjustWindowRect(
		&clientRect,
		WS_OVERLAPPEDWINDOW,	// Has a title bar, border, min and max buttons, etc.
		false);					// No menu bar

								// Center the window to the screen
	RECT desktopRect;
	GetClientRect(GetDesktopWindow(), &desktopRect);
	int centeredX = (desktopRect.right / 2) - (clientRect.right / 2);
	int centeredY = (desktopRect.bottom / 2) - (clientRect.bottom / 2);

	// Actually ask Windows to create the window itself
	// using our settings so far.  This will return the
	// handle of the window, which we'll keep around for later
	m_hWnd = CreateWindow(
		wndClass.lpszClassName,
		"",
		WS_OVERLAPPEDWINDOW,
		centeredX,
		centeredY,
		clientRect.right - clientRect.left,	// Calculated width
		clientRect.bottom - clientRect.top,	// Calculated height
		0,			// No parent window
		0,			// No menu
		m_hInstance,	// The app's handle
		0);			// No other windows in our application

					// Ensure the window was created properly
	if (m_hWnd == NULL)
	{
		DWORD error = GetLastError();
		return HRESULT_FROM_WIN32(error);
	}

	updateProjectionMatrix(NEAR_Z, FAR_Z);

	// The window exists but is not visible yet
	// We need to tell Windows to show it, and how to show it
	ShowWindow(m_hWnd, SW_MAXIMIZE);


	// Return an "everything is ok" HRESULT value
	return S_OK;
}

HWND Window::getWindowHandle()
{
	return m_instance->m_hWnd;
}

DirectX::XMFLOAT4X4 Window::getProjectionMatrix()
{
	return m_instance->m_projectionMatrix;
}

unsigned int Window::getWidth()
{
	return m_instance->m_width;
}

unsigned int Window::getHeight()
{
	return m_instance->m_height;
}

void Window::resize(unsigned int width, unsigned int height)
{
	m_instance->m_width = width;
	m_instance->m_height = height;

	m_instance->updateProjectionMatrix(NEAR_Z, FAR_Z);
}

void Window::setTitleText(std::string text)
{
	SetWindowText(m_hWnd, text.c_str());
}

void Window::updateProjectionMatrix(float nearZ, float farZ)
{
	XMMATRIX perspective = XMMatrixPerspectiveFovLH(XM_PIDIV4, (float)m_width / m_height, nearZ, farZ);
	XMStoreFloat4x4(&m_projectionMatrix, perspective);
}
