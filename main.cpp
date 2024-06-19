#include <iostream>
#include "Game.h"

int main() {
    system("chcp 65001");
    clearScreen();
    Game cardGame;
    cardGame.init();
    cardGame.play();
}
