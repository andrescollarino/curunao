#include "Engine/Application.h"


namespace Engine
{
  LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

  Application::Application(void)
  {
    m_hwnd = nullptr;
    m_hinstance = nullptr;
    m_full_screen = false;
    m_screen_height = 0;
    m_screen_width = 0;
  }

  Application::~Application(void)
  {
    Release();
  }

  bool Application::Initialize(char * screenTitle, int width, int height, bool fullScreen)
  {
    HWND hwnd; // Ventana
    WNDCLASSEX wc;

    m_hinstance = GetModuleHandle(NULL);

    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = m_hinstance;
    wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
    wc.hIconSm = wc.hIcon;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = screenTitle;
    wc.cbSize = sizeof(WNDCLASSEX);

    // Registro la clase 
    if (!RegisterClassEx(&wc))
    {
      MessageBox(NULL, "RegisterClassEx Failed", "Error", MB_OK);
      return false;
    }


    // Full Screen
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int x = (screenWidth - width) / 2 ; // Centro de la pantalla
    int y = (screenHeight - height) / 2;

    if (fullScreen)
    {
      DEVMODE dmScreenSettings;

      ZeroMemory(&dmScreenSettings, sizeof(DEVMODE));

      dmScreenSettings.dmSize = sizeof(DEVMODE);
      dmScreenSettings.dmPelsWidth = (unsigned long)width;
      dmScreenSettings.dmPelsHeight = (unsigned long)height;
      dmScreenSettings.dmBitsPerPel = 32;
      dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

      ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
    }
    else
    {
      screenWidth = width;
      screenHeight = height;
    }

    // Creo la ventana
    int nStyle = WS_OVERLAPPED | WS_SYSMENU | WS_VISIBLE | WS_CAPTION | WS_MINIMIZEBOX;

    hwnd = CreateWindowEx(
      WS_EX_APPWINDOW,
      screenTitle,
      screenTitle,
      nStyle,
      x,
      y,
      screenWidth,
      screenHeight,
      NULL,
      NULL,
      m_hinstance,
      NULL
      );


    if (hwnd == NULL)
    {
      MessageBox(NULL, "CreateWindowEx Failed", "Error", MB_OK);
      PostQuitMessage(0);
      return false;
    }

    ShowWindow(hwnd, SW_SHOW);
    SetForegroundWindow(hwnd);
    SetFocus(hwnd);

    m_full_screen = fullScreen;
    m_hwnd = hwnd;
    m_screen_height = screenHeight;
    m_screen_width = screenWidth;
    m_screen_title = screenTitle;

    return true;
  }

  AppEvents Application::Update(void)
  {
    MSG msg;
    AppEvents result;

    if (PeekMessage(&msg, NULL ,0, 0, PM_REMOVE))
    {

      // Si es el evento de salir lo informo
      if (msg.message == WM_QUIT)
      {
        result = AppEvents::Close;
      }
      else
      {
        result = AppEvents::PendingEvent;
      }

      // Bucle de mensajes en windows
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
    else
    {
      result = AppEvents::Render;
    }

    return result;
  }

  void Application::Release(void)
  {
    if (m_full_screen)
    {
      // Quito el full Screen
      ChangeDisplaySettings(NULL, 0);
      m_full_screen = false;
    }

    if (m_hinstance != nullptr)
    {
      UnregisterClass(m_screen_title.c_str(), m_hinstance);
    }

    m_hwnd = nullptr;
    m_hinstance = nullptr;
    m_full_screen = false;
    m_screen_height = 0;
    m_screen_width = 0;
  }

  LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
  {

    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
    case WM_KEYDOWN:
      if (wParam == VK_ESCAPE)
      {
        PostQuitMessage(0);
        DestroyWindow(hwnd);
      }
      break;
    case WM_PAINT:

      hdc = BeginPaint(hwnd, &ps);
      EndPaint(hwnd, &ps);

      break;
    case WM_CLOSE:
      PostQuitMessage(0);
      DestroyWindow(hwnd);
      break;
    default:
      return DefWindowProc(hwnd, message, wParam, lParam);
      break;
    }

    return 0;
  }
}

