#include "Engine/Game.h"

namespace Engine
{
  Game::Game(void)
  {
    application = new Application();
  }

  void Game::Run(void)
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
        // Render Game
      }
    }

    application->Release();
  }
}