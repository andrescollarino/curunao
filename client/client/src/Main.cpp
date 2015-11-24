#include "Engine\Game.h"
#include "Engine\Direct3D.h"

using namespace Engine;

class ArgentumOnline : public Engine::Game
{
public:
  ArgentumOnline();

  void Run();
};

ArgentumOnline::ArgentumOnline() : Engine::Game()
{
  application->Initialize("Testing", 1280, 768, false);
  Engine::Direct3D::Initialize(1280, 768, false, application->GetHWND(), false);
}


void ArgentumOnline::Run()
{
  bool run = true;
  AppEvents appEvent;

  while (run)
  {
    appEvent = application->Update();

    if (appEvent == AppEvents::Close)
    {
      run = false;
    }

    if (appEvent == AppEvents::Render)
    {
      Direct3D::BeginScene(1, 0, 1, 1);

      Direct3D::EndScene();
    }
  }

  Direct3D::Release();
  application->Release();
}



int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR  lpCmdLine, int  nCmdShow)
{
 
  ArgentumOnline * app = new ArgentumOnline();

  app->Run();

  return 0;
}