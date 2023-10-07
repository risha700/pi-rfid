#include "App.h"


int main(int argc, char* argv[])
{
    auto application =  App::create();

    return application->run(argc, argv);
}