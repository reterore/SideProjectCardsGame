//
// Created by lucas on 16/06/2024.
//

#ifndef SIDEPROJECTCARDSGAME_ARTEFACT_H
#define SIDEPROJECTCARDSGAME_ARTEFACT_H

#include <memory>
#include <string>
#include "Player.h"
#include "Fight.h"

class Player;
class Fight;
class Enemy;


class Artefact {
protected:
    string m_name;
    string m_description;
    string m_visual;
    bool m_cursed;
    static std::shared_ptr<Player> m_player;
    static std::shared_ptr<Enemy> m_enemy;
    static Fight* m_fight;


public:
    Artefact(string name, string description, string visual) : m_name(name), m_description(description), m_visual(visual) {
        m_cursed = false;
    }

    virtual ~Artefact(){
        m_fight = nullptr;
    }

    string getName(){
        return m_name;
    }

    string getDescription(){
        return m_description;
    }

    virtual string getVisual(){
        return m_visual;
    }

    virtual void triggerStartTurn() {}

    virtual void triggerAfterCard() {}

    void setEverything(std::shared_ptr<Player> player, std::shared_ptr<Enemy> enemy, Fight* fight){
        m_player = player;
        m_enemy = enemy;
        m_fight = fight;
    }

};

class ShieldArtefact : public Artefact{
protected:
    int m_value;
public:
    ShieldArtefact(string name, string description, string visual, int value)
            : Artefact(name, description, visual), m_value(value) {}

    void triggerStartTurn() override {}

    void triggerAfterCard() override {}
};



class BonusShieldArtefact : public ShieldArtefact {
public:
    BonusShieldArtefact(string name, string description, string visual, int value)
            : ShieldArtefact(name, description, visual, value) {}

    void triggerStartTurn();
};
#endif //SIDEPROJECTCARDSGAME_ARTEFACT_H
