#include "vk_jello.h"
#include "jello.h"
#include "input.h"

#if VULKAN_BUILD
int main(int argc, char** argv)
{
    if (argc < 2)
    {
        printf("Oops! You didn't say the jello world file!\n");
        printf("Usage: %s [worldfile]\n", argv[0]);
        exit(0);
    }

    readWorld(argv[1], &jello);

    Vk_Jello app;
    app.jello = jello;

    try
    {
        app.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
#endif // #if VULKAN_BUILD

