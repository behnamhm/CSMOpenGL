#include <iostream>
#include "Application.h"

int main()
{
    Application App;
    if (App.Init(1920, 1080))
    {
       App.Run();
    }

    return 0;
}


