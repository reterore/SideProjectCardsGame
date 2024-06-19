#ifndef SIDEPROJECTCARDSGAME_GAME_H
#define SIDEPROJECTCARDSGAME_GAME_H

#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>
#include "Player.h"
#include "Card.h"
#include "Event.h"
#include "Fight.h"
#include "Artefact.h"
#include <random>

using json = nlohmann::json;

// Déclarations anticipées des classes
class Player;
class Enemy;
class Boss;
class Warrior;
class Mage;

class Card;
class PassiveCard;
class CombatCard;
class SkillCard;
class SkillCard_SimpleHeal;

class Event;

std::vector<std::shared_ptr<Card>> loadCardsFromJson(const std::string& filename, const std::string& playerClass, const std::string& deckType);

class Game {

    std::shared_ptr<Player> m_player;
    std::vector<shared_ptr<Enemy>> m_enemies_list;
    std::vector<shared_ptr<Event>> m_lucky_events_list;
    std::vector<shared_ptr<Event>> m_unlucky_events_list;
    std::vector<std::shared_ptr<Card>> m_startingDeck;
    std::vector<std::shared_ptr<Card>> m_boostingDeck;
    std::vector<std::shared_ptr<Artefact>> m_cursedArtifacts;
    std::vector<std::shared_ptr<Artefact>> m_artifacts;

    int m_stage;
    int m_difficulty;

public:
    Game() : m_stage(1), m_difficulty(1) {};

    ~Game() {

    }

    void init();

    void play();

    void loadCardCollection(const std::string& filename, const std::string& playerClass);

    void addEnemiesFromBestiary(const std::string& filename, int difficulty);

    void print_enemies_list() const;
    void print_lucky_events_list() const;
    void print_unlucky_events_list() const;

    void addEventsFromJson(const std::string& filename, int difficulty);

    void loadArtifactsFromJson(const std::string& filename);

    void displayPaths(int difficulty);

    void chooseCardFromBoostDeck();

    void chooseArtefact();
};

#endif // SIDEPROJECTCARDSGAME_GAME_H
