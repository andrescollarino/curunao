#pragma once

#include "Application.h"

namespace Engine
{
  class Game
  {
  public:

    Game(void);
    virtual void Run(void);

  protected:

    Application * application;

  };
}
