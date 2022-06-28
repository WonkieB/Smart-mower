#include <iostream>

#include "mower.hpp"

int main(){
    std::cout << "Hello world!" << std::endl;

    Lawn my_lawn("basic_map.txt");
    my_lawn.display_map();
    my_lawn.move_mower(up);
    return 0;
}

/*

int main() {
    Mow mower;
    QLearner agent;
    agent.learn(mower, 1000);
    agent.print();
    agent.play(mower);
    return 0;
}
*/