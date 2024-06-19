#ifndef CARDSGAMESIDEPROJECT_CARD_H
#define CARDSGAMESIDEPROJECT_CARD_H

#include <vector>
#include <iostream>
#include <string>
#include <memory>
#include "Fight.h"

class Player;
class Enemy;
class Fight;

using namespace std;

class Card : public enable_shared_from_this<Card> {
protected:
    int m_manaCost;
    string m_name;
    string m_type;
    bool m_uniqueUse;
    static std::shared_ptr<Player> m_player;
    static std::shared_ptr<Enemy> m_enemy;
    static Fight* m_fight;

public:
    // Constructor
    Card(int manaCost, const string& name, const string& type, bool uniqueUse)
            : m_manaCost(manaCost), m_name(name), m_type(type), m_uniqueUse(uniqueUse) {}

    // Destructor
    virtual ~Card() {
        if (m_fight) {
            m_fight = nullptr;
        }
    }

    // Getters
    int getManaCost() const { return m_manaCost; }
    const string& getName() const { return m_name; }

    virtual string getDescription() {
        string result = "This card does this.";
        return result;
    }

    virtual void setPlayer(std::shared_ptr<Player> player) {
        m_player = player;
    }
    virtual void setEnemy(std::shared_ptr<Enemy> enemy) {
        m_enemy = enemy;
    }
    virtual void setFight(Fight* fight) {
        m_fight = fight;
    }

    virtual void play();

    bool isUniqueUse() const { return m_uniqueUse; }

    void remove();
};

class PassiveCard : public Card {
protected:
    bool m_played;
public:
    // Constructor
    PassiveCard(int manaCost, const string& name, bool uniqueUse)
            : Card(manaCost, name, "passive", uniqueUse), m_played(false) {}

    // Destructor
    ~PassiveCard() override {}

    virtual void play() override;

    virtual void doPassiveEffect() {}
};

class PassiveDamageCard : public PassiveCard {
private:
    int m_damage;
public:
    PassiveDamageCard(int manaCost, const string& name, bool uniqueUse, int damage)
            : PassiveCard(manaCost, name, uniqueUse), m_damage(damage) {}

    ~PassiveDamageCard() override {}

    string getDescription() override {
        string result = "This card does " + to_string(m_damage) + " damages per turn";
        return result;
    }

    void doPassiveEffect() override;
};

class CombatCard : public Card {
private:
    int m_damage;
    int m_numAttacks;

public:
    // Constructor
    CombatCard(int manaCost, const string& name, int damage, int numAttacks = 1, bool uniqueUse = false)
            : Card(manaCost, name, "combat", uniqueUse), m_damage(damage), m_numAttacks(numAttacks) {}

    // Destructor
    ~CombatCard() override {}

    // Getters
    int getDamage() const { return m_damage; }
    int getNumAttacks() const { return m_numAttacks; }

    string getDescription() override;

    // Override the use method
    void play() override;
};

class SkillCard : public Card {
public:
    // Constructor
    SkillCard(int manaCost, const string& name, bool uniqueUse)
            : Card(manaCost, name, "skill", uniqueUse) {}

    // Destructor
    ~SkillCard() override {}

    string getDescription() override {
        return "This is a skill card.";
    }

    void play() override;
};

class SkillCard_SimpleHeal : public SkillCard {
private:
    int m_numHeals; // Nombre de guérisons
    int m_healValue; // Valeur de guérison

public:
    // Constructor
    SkillCard_SimpleHeal(int manaCost, const string& name, int healValue, int numValue, bool uniqueUse)
            : SkillCard(manaCost, name, uniqueUse), m_healValue(healValue), m_numHeals(numValue) {}

    // Destructor
    ~SkillCard_SimpleHeal() override {}

    string getDescription() override;

    // Override the use method
    void play() override;
};

class SkillCard_SimpleShield : public SkillCard {
private:
    int m_value;

public:
    // Constructor
    SkillCard_SimpleShield(int manaCost, const string& name, int value, bool uniqueUse)
            : SkillCard(manaCost, name, uniqueUse), m_value(value) {}

    // Destructor
    ~SkillCard_SimpleShield() override {}

    string getDescription() override {
        return "This card provides " + to_string(m_value) + " shield.";
    }

    // Override the use method
    void play() override;
};

#endif //CARDSGAMESIDEPROJECT_CARD_H
