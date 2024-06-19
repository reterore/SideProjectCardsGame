//
// Created by lucas on 10/06/2024.
//

#include "Event.h"

void Event::trigger() {
    std::cout << getName() << ": " << getDescription() << std::endl;
}

void LuckyEvent_freeCardChoice::trigger() {
    Event::trigger();
    m_game->chooseCardFromBoostDeck();
}

void LuckyEvent_ArtefactGoblin::trigger() {
    Event::trigger();
    m_game->chooseArtefact();
}


// unluckyEvent here:
void UnluckyEvent_Trap::trigger() {
    Event::trigger();
    for (int i = 0; i < m_numberOfDoors; ++i) {
        std::cout << " ______   ";
    }
    cout << "\n";
    for (int i = 0; i < m_numberOfDoors; ++i) {
        std::cout << "|      |  ";
    }
    cout << "\n";
    for (int i = 0; i < m_numberOfDoors; ++i) {
        std::cout << "|    " << i+1 << " |  ";
    }
    cout << "\n";
    for (int i = 0; i < m_numberOfDoors; ++i) {
        std::cout << "|      |  ";
    }
    cout << "\n";
    for (int i = 0; i < m_numberOfDoors; ++i) {
        std::cout << "|______|  ";
    }

    int playerChoice;
    std::cout << "\nChoose a door (1 - " << m_numberOfDoors << ")\n: ";
    std::cin >> playerChoice;

    // Validation du choix du joueur
    while (true) {
        // Vérifier si l'entrée est un nombre entier
        if (std::cin.fail()) {
            std::cin.clear(); // Efface l'état de défaillance
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignore les caractères invalides
            std::cout << "Invalid input. Please enter a number between 1 and " << m_numberOfDoors << ".\n: ";
        } else if (playerChoice < 1 || playerChoice > m_numberOfDoors) {
            std::cout << "Invalid choice. Please choose a door (1 - " << m_numberOfDoors << ").\n: ";
        } else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignore les caractères restants
            break;
        }
        std::cin >> playerChoice;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(1, m_numberOfDoors);
    int randomChoice = distrib(gen);

    if (playerChoice != randomChoice) {
        m_player->takeDamage(m_damage); // Assuming Player has a takeDamage method
        clearScreen();
        std::cout << "You chose the wrong door and took " << m_damage << " damage!" << std::endl;
    } else {
        std::cout << "You opened the right door." << std::endl;
    }
}