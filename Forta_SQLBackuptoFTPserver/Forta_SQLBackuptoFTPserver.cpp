#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string_view>
#include <string>
// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#include <iostream>

#include "CLIApplication.h"

int main(int argc, char** argv)
{
    CLIApplication app;
    auto args = CLIArguments::parseArgs(argc, argv);
    if (!args.error.empty()) {
        std::cout << args.error << std::endl;
        std::cout << app.help() << std::endl;
        return 1;
    }
    if (args.help) {
        std::cout << app.help() << std::endl;
        return 0;
    }

    return app.exec(args);
}