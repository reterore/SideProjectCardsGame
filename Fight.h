//
// Created by lucas on 17/05/2024.
//

#ifndef SIDEPROJECTCARDSGAME_FIGHT_H
#define SIDEPROJECTCARDSGAME_FIGHT_H

#include <memory>
#include <vector>
#include "Player.h"
#include "Card.h"
#include "iostream"

using namespace std;

class Player;
class Card;
class Enemy;

class Fight {
private:
    std::shared_ptr<Player> m_player;
    std::shared_ptr<Enemy> m_enemy;
    std::vector<std::shared_ptr<Card>> m_passiveDeck;
    int m_turn;

public:
    // Constructeur qui initialise les pointeurs de joueur et d'ennemi
    Fight(std::shared_ptr<Player> player, std::shared_ptr<Enemy> enemy);

    // Destructeur
    ~Fight();

    void innit();
    // Méthode pour démarrer le combat
    void play();

    // Méthode pour terminer le combat
    void endFight();

    // Méthode pour le tour du joueur
    void playerTurn();

    // Méthode pour le tour de l'ennemi
    void enemyTurn();

    // Méthode pour afficher l'interface de combat
    void displayFightInterface(shared_ptr<Card> chosenCard);
    void displayFightInterface();

    void playPassivs();

    void clearPassivDeck();

    void addToPassivDeck(shared_ptr<Card> card);

    void removeToPassivDeck(shared_ptr<Card> card);

    // Méthode pour obtenir le numéro du tour actuel
    int getTurn() const;
};

#endif //SIDEPROJECTCARDSGAME_FIGHT_H
