//
// Created by lucas on 10/06/2024.
//

#ifndef SIDEPROJECTCARDSGAME_EVENT_H
#define SIDEPROJECTCARDSGAME_EVENT_H

#include "string"
#include "iostream"
#include "Player.h"
#include "Game.h"
#include <random>

class Player;
class Game;

class Event {
protected:
    std::string m_name;
    std::string m_description;
    std::shared_ptr<Player> m_player;
    Game* m_game;

public:
    // Constructeur
    Event(const std::string& name, const std::string& description)
            : m_name(name), m_description(description) {}

    // Accesseur pour le nom
    std::string getName() const {
        return m_name;
    }

    // Mutateur pour le nom
    void setName(const std::string& name) {
        m_name = name;
    }

    // Accesseur pour la description
    std::string getDescription() const {
        return m_description;
    }

    // Mutateur pour la description
    void setDescription(const std::string& description) {
        m_description = description;
    }

    void setPlayer(std::shared_ptr<Player> player){
        m_player = player;
    }

    void setGame(Game* game){
        m_game = game;
    }

    virtual void trigger() = 0;
};

// luckyEvent here:
class LuckyEvent_freeCardChoice : public Event {
public:
    LuckyEvent_freeCardChoice(const std::string& name, const std::string& description)
            : Event(name, description) {}

    void trigger() override;
};

class LuckyEvent_ArtefactGoblin : public Event {
public:
    LuckyEvent_ArtefactGoblin(const std::string& name, const std::string& description)
            : Event(name, description) {}

    void trigger() override;
};

// unluckyEvent here:
class UnluckyEvent_Trap : public Event {
    int m_numberOfDoors;
    int m_damage;
public:
    UnluckyEvent_Trap(const std::string &name, const std::string &description, const int numberOfDoors, const int damage)
            : Event(name, description), m_numberOfDoors(numberOfDoors), m_damage(damage) {}

    void trigger() override;
};

#endif //SIDEPROJECTCARDSGAME_EVENT_H
