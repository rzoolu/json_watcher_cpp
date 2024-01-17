#include <Log.h>

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
    setLogLevel(DEBUG);

    LOG(ERROR, "Hello main() error \"{:6}\" \"{:.5f}\" \"{:d}\" \"{:x}\"", 1, 1.0 / 3.0, 42, 255);

    LOG(DEBUG, "Hello main() error \"{:6}\" \"{:.5f}\" \"{:d}\" \"{:x}\"", 1, 1.0 / 3.0, 42, 255);

    return 1;
}
