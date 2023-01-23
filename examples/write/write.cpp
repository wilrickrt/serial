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

    if (auto serial = wrt::OpenSerial(argv[1], argv[2]); serial)
    {
        while (!(GetAsyncKeyState('Q') & 1))
        {
            if (GetAsyncKeyState('1') & 1)
            {
                serial->Write("1");
            }
            if (GetAsyncKeyState('0') & 1)
            {
                serial->Write("0");
            }
        }
    }
    else
    {
        std::cout << "Failed to open the serial bus\n";
    }
}
