#include "Game.h"
#include <iostream>

int main(int argc, char* argv[]) {
    std::cout << "Starting 3D Frogger game..." << std::endl;
    
    try {
        Game game;
        game.run();
    } catch (const std::exception& e) {
        std::cerr << "Exception occurred: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "Game ended." << std::endl;
    return 0;
}