#include "wrt/serial/serial.hpp"

#include <iostream>

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        std::cout << "Run the program as follows: \n\t"
            << argv[0]
            << " <vendor id> <product id>\n"
            << "Note: the ids are expected to be hexadecimal\n";
        return 1;
    }

    if (auto com = wrt::FindComPort(argv[1], argv[2]))
    {
        std::cout << "COM port: " << *com << std::endl;
    }
    else
    {
        std::cout << "Failed to find the COM port\n";
    }
}
