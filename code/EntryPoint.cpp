#include <memory>

#include "Engine.h"
#include "ApplicationSandbox.h"

int main(int argc, char **argv)
{
    using namespace BGLRenderer;

    std::shared_ptr<Engine> engine = std::make_shared<Engine>(argc, argv);
    engine->setApplication<ApplicationSandbox>();
    return engine->run();
}
