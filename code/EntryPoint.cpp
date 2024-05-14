#include <memory>

#include "ApplicationSandbox.h"

int main(int argc, char **argv)
{
    using namespace BGLRenderer;
    std::shared_ptr<ApplicationSandbox> app = std::make_shared<ApplicationSandbox>(argc, argv);
    return app->run();
}
