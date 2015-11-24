#pragma once

#include <Windows.h>
#include <string>

namespace Engine
{
  enum class AppEvents
  {
    Close = 0,
    Render,
    PendingEvent,
  };

  class Application
  {
  public:

    Application(void);
    ~Application(void);

    bool Initialize(char * screenTitle, int width, int height ,bool fullScreen);
    AppEvents Update(void);
    void Release(void);

    inline HWND GetHWND() { return m_hwnd; }
    inline bool IsFullScreen() { return m_full_screen; }
    inline int GetWidth() { return m_screen_width; }
    inline int GetHeight() { return m_screen_height; }
    inline std::string GetTitle() { return m_screen_title; }

  private:

    HWND m_hwnd;
    int m_screen_width;
    int m_screen_height;
    bool m_full_screen;
    std::string m_screen_title;
    HINSTANCE m_hinstance;
  };
}