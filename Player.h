#ifndef CARDSGAMESIDEPROJECT_PLAYER_H
#define CARDSGAMESIDEPROJECT_PLAYER_H

#include <vector>
#include <iostream>
#include <algorithm>
#include <string>
#include <memory>
#include <iomanip>
#include <random>
#include "Card.h"
#include "Artefact.h"

#define MAX_CARD_IN_HAND 12

class Card;
class Artefact;
class Fight;

void clearScreen();

class Player : public enable_shared_from_this<Player>{
protected:
    std::string m_name;
    int m_maxHealth;
    int m_currentHealth;
    int m_power;
    int m_shield;
    int m_extraLife;
    int m_maxMana;
    int m_currentMana;
    int m_gold;
    int m_weakness;
    std::vector<std::shared_ptr<Artefact>> m_artefacts;
    std::vector<std::shared_ptr<Card>> m_exileDeck;
    std::vector<std::shared_ptr<Card>> m_baseDeck;
    std::vector<std::shared_ptr<Card>> m_combatDeck;
    std::vector<std::shared_ptr<Card>> m_hand;
    std::vector<std::shared_ptr<Card>> m_graveyard;

public:
    Player(const std::string& name, int maxHealth, int power)
            : m_name(name), m_maxHealth(maxHealth), m_currentHealth(maxHealth),
              m_power(power), m_shield(0), m_extraLife(0), m_currentMana(0), m_gold(0), m_maxMana(5), m_weakness(0) {}

    virtual ~Player() = default;

    const std::string& getName() const { return m_name; }
    int getMaxHealth() const { return m_maxHealth; }
    int getCurrentHealth() const { return m_currentHealth; }
    int getPower() const { return m_power; }
    int getShield() const { return m_shield; }
    int getExtraLife() const { return m_extraLife; }
    int getGold() const { return m_gold; }
    int getWeakness() const { return m_weakness; }
    std::shared_ptr<Card> getCardFromHand(int num) const {
        if (num >= 0 && num < m_hand.size()) {
            return m_hand[num];
        } else {
            return nullptr;
        }
    }
    int getMaxMana() const { return m_maxMana; }
    int getCurrentMana() { return m_currentMana; }
    int getHandSize() const { return m_hand.size(); }
    void takeDamage(int dmg);
    void showHand() const;
    void showDeck() const;
    bool isDefeated() const;
    void playCard(shared_ptr<Card> card);
    bool emptyHand() const;
    bool emptyDeck() const;
    virtual void display() const;
    void heal(int heal, int nbHeal);
    void addMana(int value);
    void removeMana(int value);
    void addShield(int value);
    void removeShield(int value);
    void clearShield();
    void resetMana(){ m_currentMana = m_maxMana; }
    void addCardToHand(const std::shared_ptr<Card>& card);
    void removeCardFromHand(const std::shared_ptr<Card>& card);
    void drawCardsFromDeck(int numCards);
    void drawCardFromDeck();
    void redoDeck();
    void flushHand();
    void innitCombatDeck();
    virtual void innitTurn();
    int calculateDamage(int value){
        int totalDamage = value + m_power;
        if (m_weakness > 0){
            totalDamage *= 0.75;
        }
        return totalDamage;
    }

    void addGold(int value){
        m_gold += value;
    }
    void RemoveGold(int value){
        m_gold -= value;
    }

    void addWeakness(int value){
        m_weakness += value;
    }
    void removeWeakness(int value){
        m_weakness -= value;
    }

    void addCardToCombatDeck(const std::shared_ptr<Card>& card);
    void addCardToBaseDeck(const std::shared_ptr<Card>& card);
    void addCardToGraveyard(const std::shared_ptr<Card>& card);
    void addCardToExile(const std::shared_ptr<Card>& card);
    void addArtefact(shared_ptr<Artefact> artefact);
    void clearExile() {
        m_exileDeck.clear();
    }
    void clearGraveyard(){
        m_graveyard.clear();
    }

    void setEnemy(shared_ptr<Enemy> enemy) {
        m_combatDeck.begin()->get()->setEnemy(enemy);
    }
    void setFight(Fight* fight) {
        m_combatDeck.begin()->get()->setFight(fight);
    }

    void setArtefacts(shared_ptr<Enemy> enemy, Fight* fight);

    void playStartTurnArtefact();

    void playAfterCardArtefact();

    void displayArtefact(int index);
};

class Warrior : public Player {
public:
    Warrior(const std::string& name, int maxHealth)
            : Player(name, maxHealth, 0) {
        m_extraLife = 0;
    }

    ~Warrior() override {}

    void display() const override{
        Player::display();
        std::cout << "Mana: ";
        for (int i = 0; i < m_maxMana; ++i) {
            if (i < m_currentMana) {
                std::cout << "(♢-)";
            } else {
                std::cout << "(  )";
            }
        }
        std::cout << " (" << m_currentMana << "/" << m_maxMana << ")";
        cout << " | Deck: " << m_combatDeck.size() << " | graveyard: " << m_graveyard.size() << " |";
        cout << " ⚔ : " << m_power;
        if (m_weakness > 0){
            cout << " | ⛓ : " << m_weakness;
        }
        cout << " |\n";
    }
};

class Mage : public Player {
public:
    Mage(const std::string& name, int maxHealth)
            : Player(name, maxHealth, 0) {
        m_extraLife = 0;
    }

    ~Mage() override {}

    void display() const override {
        Player::display();
        std::cout << "Mana: ";
        for (int i = 0; i < m_maxMana; ++i) {
            if (i < m_currentMana) {
                std::cout << "(♢-)";
            } else {
                std::cout << "(  )";
            }
        }
        std::cout << " (" << m_currentMana << "/" << m_maxMana << ")";
        cout << " | Deck: " << m_combatDeck.size() << " | graveyard: " << m_graveyard.size() << " |";
        cout << " ⚔ : " << m_power;
        if (m_weakness > 0){
            cout << " | ⛓ : " << m_weakness;
        }
        cout << " |\n";
    }
};

class Enemy : public Player {
protected:
    static std::shared_ptr<Player> m_player;
    int m_numberOfAttack;
    int m_AttackChoice;
public:
    Enemy(const std::string& name, int health , int power, int numberOfAttack)
            : Player(name, health, power), m_AttackChoice(0), m_numberOfAttack(numberOfAttack) {
    }

    ~Enemy() override {}


    void innitTurn() override {
        // Utilisation de std::random_device pour obtenir un seed indépendant
        std::random_device rd;
        // Utilisation de std::mt19937 pour générer des nombres pseudo-aléatoires
        std::mt19937 gen(rd());
        // Définir la distribution entre 1 et m_numberOfAttack
        std::uniform_int_distribution<> dis(1, m_numberOfAttack);

        // Choisir une attaque aléatoire entre 1 et m_numberOfAttack
        m_AttackChoice = dis(gen);
    }

    virtual void playTurn() {
        cout << m_name << " turn!\n";
    }

    // return Enemy::AttackMessage() for plain damaging attack
    virtual string AttackMessage(){
        int totalDamage = calculateDamage(0);
        return m_name + " is going to attack for " + to_string(totalDamage) + " damages";
    }

    void setPlayer(std::shared_ptr<Player> player){
        m_player = player;
    }

    void rawAttacks(){
        int totalDamage = calculateDamage(0);
        m_player->takeDamage(totalDamage);
    }

    virtual void show() {}

    virtual bool isBoss() {
        return false;
    }
};

class Boss : public Enemy{
public:
    Boss(const std::string& name, int health , int power, int numberOfAttack) :
            Enemy(name, health, power, numberOfAttack) {}

    ~Boss() override {}

    bool isBoss() override{
        return true;
    }
};

class Goblin : public Enemy {
public:
    Goblin(const std::string& name, int health , int power)
            : Enemy(name, health, power, 1) {
    }

    ~Goblin() override {}

    void playTurn() override {
        Enemy::playTurn();
        rawAttacks();
    }

};

class Ghoul : public Enemy {
public:
    Ghoul(const std::string& name, int health , int power)
            : Enemy(name, health, power, 1) {
    }

    ~Ghoul() override {}


    void playTurn() override {
        Enemy::playTurn();
        rawAttacks();
    }
};

class Liche : public Boss {
public:
    Liche(const std::string& name, int health , int power)
            : Boss(name, health, power, 3) {
    }

    ~Liche() override {}

    string AttackMessage() override{
        if (m_AttackChoice == 1){
            int totalDamage = calculateDamage(0);
            return "The liche is Going to attack you for " + to_string(totalDamage) + " damages, and heals for " +
                    to_string(totalDamage);
        } else if(m_AttackChoice == 2) {
            return "The liche will cast a weakness spell on you lasting for 2 attacks";
        } else {
            return "The liche will cast a magic shield spell on her";
        }

    }

    void playTurn() override {
        Enemy::playTurn();
        if (m_AttackChoice == 1){
            cout << "The Lich Sucked your life Force!\n";
            rawAttacks();
            int totalHeal = calculateDamage(0);
            this->heal(totalHeal, 1);
        } else if (m_AttackChoice == 2){
            cout << "You got Weakend for 2 attacks!\n";
            m_player->addWeakness(2);
        } else {
            this->addShield(50);
        }

    }
};

#endif // CARDSGAMESIDEPROJECT_PLAYER_H
