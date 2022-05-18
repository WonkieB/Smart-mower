#include <iostream>

#include "mower.hpp"

int main(){
    std::cout << "Hello world!" << std::endl;

    Lawn my_lawn("basic_map.txt");
    my_lawn.display_map();
    my_lawn.move_mower(up);
    return 0;
}