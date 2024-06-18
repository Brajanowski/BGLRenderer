#include <memory>

#include <Foundation/Engine.h>
#include <Sandbox/ApplicationSandbox.h>

int main(int argc, char **argv)
{
    using namespace BGLRenderer;

    auto engine = std::make_shared<Engine>(argc, argv);
    engine->setApplication<ApplicationSandbox>();
    return engine->run();
}
