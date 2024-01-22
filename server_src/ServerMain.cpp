#include <FileMonitorI.h>
#include <Log.h>

int main(int argc, char* argv[])
{
    setLogLevel(DEBUG);

    LOG(DEBUG, "Server started, argc = {}", argc);
    for (auto i = 0; i < argc; ++i)
    {
        LOG(DEBUG, "argv[{}] = {},", i, argv[i]);
    }

    return 1;
}
