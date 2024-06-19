// Created by lucas on 17/05/2024.

#define MAX_DIFFICULTY 3

#include "Game.h"
#include "Event.h"
#include <fstream>
#include <algorithm>
#include <random>
#include <nlohmann/json.hpp>


void Game::addEnemiesFromBestiary(const std::string& filename, int difficulty) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Unable to open bestiary file: " + filename);
    }

    json bestiaryData;
    file >> bestiaryData;

    std::vector<std::shared_ptr<Enemy>> allEnemies;
    std::vector<std::shared_ptr<Enemy>> allBosses;
    int difficultyHealthBonus = (difficulty - 1) * 30;
    int difficultyPowerBonus = (difficulty - 1) * 5;

    for (const auto& entry : bestiaryData) {
        std::string name = entry["name"];
        int health = entry["health"];
        int attack = entry["attack"];
        bool isBoss = entry["boss"];
        std::string type = entry["type"];

        health += difficultyHealthBonus;
        attack += difficultyPowerBonus;

        std::shared_ptr<Enemy> enemy;

        if (type == "Goblin") {
            enemy = std::make_shared<Goblin>(name, health, attack);
        } else if (type == "Ghoul") {
            enemy = std::make_shared<Ghoul>(name, health, attack);
        } else if (type == "Liche") {
            enemy = std::make_shared<Liche>(name, health, attack);
        } else {
            throw std::runtime_error("Unknown enemy type: " + type);
        }

        if (isBoss) {
            allBosses.push_back(enemy);
        } else {
            allEnemies.push_back(enemy);
        }
    }

    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (!allEnemies.empty()) {
                int randomIndex = std::rand() % allEnemies.size();
                m_enemies_list.push_back(allEnemies[randomIndex]);
                allEnemies.erase(allEnemies.begin() + randomIndex);
            }
        }

        if (!allBosses.empty()) {
            int randomIndex = std::rand() % allBosses.size();
            m_enemies_list.push_back(allBosses[randomIndex]);
            allBosses.erase(allBosses.begin() + randomIndex);
        }
    }
    if (!m_enemies_list.empty()) {
        m_enemies_list.begin()->get()->setPlayer(m_player);
    }
}

void Game::loadCardCollection(const std::string& filename, const std::string& playerClass) {
    auto loadCardsFromJson = [&](const std::string& filename, const std::string& playerClass, const std::string& deckType) {
        std::vector<std::shared_ptr<Card>> cards;
        std::ifstream file(filename);

        if (!file.is_open()) {
            throw std::runtime_error("Unable to open file: " + filename);
        }

        json cardData;
        try {
            file >> cardData;
        } catch (const json::parse_error& e) {
            throw std::runtime_error("JSON parse error in file " + filename + ": " + std::string(e.what()));
        }

        for (const auto& card : cardData) {
            std::string type = card["type"];
            std::string name = card["name"];
            int manaCost = card["manaCost"];
            bool uniqueUse = card["uniqueUse"];
            std::vector<std::string> classes = card["classes"];
            std::string cardDeckType = card["deckType"];
            int count = card.value("count", 1);  // Default count to 1 if not specified

            if (std::find(classes.begin(), classes.end(), playerClass) != classes.end() && cardDeckType == deckType) {
                std::shared_ptr<Card> cardPtr;

                if (type == "CombatCard") {
                    int damage = card["damage"];
                    int numAttacks = card["numAttacks"];
                    cardPtr = std::make_shared<CombatCard>(manaCost, name, damage, numAttacks, uniqueUse);
                } else if (type == "SkillCard_SimpleHeal") {
                    int healValue = card["healValue"];
                    int numHeals = card["numHeals"];
                    cardPtr = std::make_shared<SkillCard_SimpleHeal>(manaCost, name, healValue, numHeals, uniqueUse);
                } else if (type == "PassiveCard") {
                    cardPtr = std::make_shared<PassiveCard>(manaCost, name, uniqueUse);
                } else if (type == "SkillCard") {
                    cardPtr = std::make_shared<SkillCard>(manaCost, name, uniqueUse);
                } else if (type == "SkillCard_SimpleShield") {
                    int value = card["value"];
                    cardPtr = std::make_shared<SkillCard_SimpleShield>(manaCost, name, value, uniqueUse);
                } else if (type == "PassiveDamageCard") {
                    int damage = card["damage"];
                    cardPtr = std::make_shared<PassiveDamageCard>(manaCost, name, uniqueUse, damage);
                }
                else {
                    throw std::runtime_error("Unknown card type: " + type);
                }

                for (int i = 0; i < count; ++i) {
                    cards.push_back(cardPtr);
                }
            }
        }

        if (!cards.empty()) {
            cards.begin()->get()->setPlayer(m_player);
        }

        return cards;
    };

    m_startingDeck = loadCardsFromJson(filename, playerClass, "starter");
    m_boostingDeck = loadCardsFromJson(filename, playerClass, "booster");
}

using json = nlohmann::json;

void Game::addEventsFromJson(const std::string& filename, int difficulty) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Unable to open events file: " + filename);
    }

    json eventsData;
    file >> eventsData;

    for (const auto& entry : eventsData) {
        std::string name = entry["name"];
        std::string description = entry["description"];
        bool isLucky = entry["lucky"];
        auto eventDifficulty = entry["eventDifficulty"];
        std::string type = entry["type"];

        bool hasDifficulty = std::find(eventDifficulty.begin(), eventDifficulty.end(), difficulty) != eventDifficulty.end();

        if (hasDifficulty) {
            std::shared_ptr<Event> event;

            if (isLucky) {
                if (type == "LuckyEvent_freeCardChoice") {
                    event = std::make_shared<LuckyEvent_freeCardChoice>(name, description);
                } else if (type == "LuckyEvent_ArtefactGoblin") {
                    event = std::make_shared<LuckyEvent_ArtefactGoblin>(name, description);
                }
                m_lucky_events_list.push_back(event);
            } else {
                if (type == "UnluckyEvent_Trap") {
                    int damage = entry["damage"];
                    int numberOfDoors = entry["numberOfDoors"];  // Correction ici
                    event = std::make_shared<UnluckyEvent_Trap>(name, description, numberOfDoors, damage);
                }
                m_unlucky_events_list.push_back(event);
            }

            event->setPlayer(m_player);
            event->setGame(this);
        }
    }

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(m_lucky_events_list.begin(), m_lucky_events_list.end(), g);
    std::shuffle(m_unlucky_events_list.begin(), m_unlucky_events_list.end(), g);
}

void Game::loadArtifactsFromJson(const std::string& filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Unable to open file: " + filename);
    }

    json artifactData;
    try {
        file >> artifactData;
    } catch (const json::parse_error& e) {
        throw std::runtime_error("JSON parse error in file " + filename + ": " + std::string(e.what()));
    }

    for (const auto& artifact : artifactData) {
        std::string type = artifact["type"];
        std::string name = artifact["name"];
        std::string description = artifact["description"];
        std::string visual = artifact["visual"];  // Assumes the visual is a single UTF-8 character
        int value = artifact["value"];
        bool cursed = artifact["cursed"];

        if (type == "BonusShieldArtefact") {
            std::shared_ptr<BonusShieldArtefact> artifactPtr = std::make_shared<BonusShieldArtefact>(name, description, visual, value);
            if (cursed) {
                m_cursedArtifacts.push_back(artifactPtr);
            } else {
                m_artifacts.push_back(artifactPtr);
            }
        } else {
            throw std::runtime_error("Unknown artifact type: " + type);
        }
    }
}

// largeur du terminal : 120 char
void Game::init() {
    std::cout << "════════════════════════════════════════════════\n";
    std::cout << "║              Welcome in this game            ║\n";
    std::cout << "════════════════════════════════════════════════\n";
    char ClassChoice;
    int difficultyChoice;
    std::string playerClass;
    do {
        std::cout << "Choose if you want to play a Warrior (w) or a Mage (m)\n: ";
        std::cin >> ClassChoice;
        if (ClassChoice == 'w') {
            m_player = std::make_shared<Warrior>("Warrior", 100);
            playerClass = "Warrior";
        } else if (ClassChoice == 'm') {
            m_player = std::make_shared<Mage>("Mage", 100);
            playerClass = "Mage";
        }
    } while (ClassChoice != 'w' && ClassChoice != 'm');

    loadCardCollection("C:\\Users\\lucas\\CLionProjects\\SideProjectCardsGame\\cards.json", playerClass);

    std::cout << "You have chosen a " << m_player->getName() << std::endl;

    do {
        std::cout << "\nChoose the difficulty for the game:\n";
        for (int i = 1; i <= MAX_DIFFICULTY - 1; ++i) {
            std::cout << i << " ---------- ";
        }
        std::cout << MAX_DIFFICULTY << "\n:";
        std::cin >> difficultyChoice;
    } while (difficultyChoice < 1 || difficultyChoice > MAX_DIFFICULTY);

    m_difficulty = difficultyChoice;
    std::cout << "You have chosen " << m_difficulty << " for the difficulty.\n";

    for (const auto& card : m_startingDeck) {
        m_player->addCardToBaseDeck(card);
    }

    addEnemiesFromBestiary("C:\\Users\\lucas\\CLionProjects\\SideProjectCardsGame\\bestiary.json", m_difficulty);

    addEventsFromJson("C:\\Users\\lucas\\CLionProjects\\SideProjectCardsGame\\Events.json", m_difficulty);

    loadArtifactsFromJson("C:\\Users\\lucas\\CLionProjects\\SideProjectCardsGame\\Artefact.json");

}

void Game::play() {
    int enemy_index = 0;
    int enemies_list_end_index = m_enemies_list.size();
    char pathChoice;

    while (enemy_index < enemies_list_end_index) {
        displayPaths(m_difficulty);
        cout << "Choose your path!\n: ";
        cin >> pathChoice;
        shared_ptr<Enemy> currentEnemy = m_enemies_list[enemy_index];
        while (pathChoice != 'z' && pathChoice != 'q' && pathChoice != 's'
               && pathChoice != '1' && pathChoice != '2' && pathChoice != '3') {
            displayPaths(m_difficulty);
            cout << "Incorrect path selected\n: ";
            cout << "Choose your path\n: ";
            cin >> pathChoice;
            clearScreen();
        }

        int choice = std::rand() % 3;

        if (choice == 0 && !m_lucky_events_list.empty()) {
            // Trigger a lucky event
            int lucky_index = std::rand() % m_lucky_events_list.size();
            clearScreen();
            m_lucky_events_list[lucky_index]->trigger();
            m_lucky_events_list.erase(m_lucky_events_list.begin() + lucky_index);
        } else if (choice == 1 && !m_unlucky_events_list.empty()) {
            // Trigger an unlucky event
            int unlucky_index = std::rand() % m_unlucky_events_list.size();
            clearScreen();
            m_unlucky_events_list[unlucky_index]->trigger();
            m_unlucky_events_list.erase(m_unlucky_events_list.begin() + unlucky_index);
        } else {
            // Trigger a combat
            Fight currentFight(m_player, currentEnemy);
            m_stage++;
            currentFight.play();
            enemy_index++;
            if (!m_player->isDefeated() && enemy_index != enemies_list_end_index){
                chooseCardFromBoostDeck();
                if (currentEnemy->isBoss()){
                    chooseArtefact();
                }
            }
        }
        if (m_player->isDefeated()){
            cout << "***YOU LOST THE GAME***\n";
            std::cout << "      ,-=-. \n";
            std::cout << "     /  +  \\  \n";
            std::cout << "  *  | ~~~ | + \n";
            std::cout << "  |  |R.I.P| | \n";
            std::cout << "  V,,|_____|,V,";
            return;
        }
    }
    clearScreen();
    cout << "**CONGRATULATION, YOU BEAT THIS GAME ON DIFFICULTY " << m_difficulty << "!**\n";
}

void Game::print_enemies_list() const {
    std::cout << "Enemies:\n";
    for (const auto& enemy : m_enemies_list) {
        std::cout << "Enemy: " << enemy->getName() << ", Health: " << enemy->getMaxHealth() << ", Attack: " << enemy->getPower() << std::endl;
    }
}

void Game::print_lucky_events_list() const {
    std::cout << "Lucky events:\n";
    for (const auto& event : m_lucky_events_list) {
        std::cout << "name: " << event->getName() << ", description: " << event->getDescription() << endl;
    }
}

void Game::print_unlucky_events_list() const {
    std::cout << "Unlucky events:\n";
    for (const auto& event : m_unlucky_events_list) {
        std::cout << "name: " << event->getName() << ", description: " << event->getDescription() << endl;
    }
}

void Game::displayPaths(int difficulty) {
    cout << m_player->getName() << " | ⛀⛁ : " << m_player->getGold() << " | stage : "<< m_stage << " |\n";
    if (difficulty == 1){
        std::cout << "|.'',       The Mysterious Dungeon        ,''.|\n";
        std::cout << "|.'.'',                                 ,''.'.|\n";
        std::cout << "|.'.'.'',                             ,''.'.'.|\n";
        std::cout << "|.'.'.'.'',                         ,''.'.'.'.|\n";
        std::cout << "|.'.'.'.'.|                         |.'.'.'.'.|\n";
        std::cout << "|.'.'.'.'.|===;                 ;===|.'.'.'.'.|\n";
        std::cout << "|.'.'.'.'.|:::|',   >>|1|<<   ,'|:::|.'.'.'.'.|\n";
        std::cout << "|.'.'.'.'.|---|'.|, _______ ,|.'|---|.'.'.'.'.|\n";
        std::cout << "|.'.'.'.'.|:::|'.|'|???????|'|.'|:::|.'.'.'.'.|\n";
        std::cout << "|,'- 2 -',|---|',|'|?* 1 *?|'|,'|---|,'- 3 -',|\n";
        std::cout << "|.'.'.'.'.|:::|'.|'|???????|'|.'|:::|.'.'.'.'.|\n";
        std::cout << "|.'.'.'.'.|---|','   /%%%\\   ','|---|.'.'.'.'.|\n";
        std::cout << "|.'.'.'.'.|===:'    /%%%%%\\    ':===|.'.'.'.'.|\n";
        std::cout << "|.'.'.'.'.|%%%%%%%%%%%%%%%%%%%%%%%%%|.'.'.'.'.|\n";
        std::cout << "|.'.'.'.','       /%%%%%%%%%\\       ','.'.'.'.|\n";
        std::cout << "|.'.'.','        /%%%%%%%%%%%\\        ','.'.'.|\n";
        std::cout << "|.'.','  <-     /%%%%%%%%%%%%%\\     ->  ','.'.|\n";
        std::cout << "|.','    2     /%%%%%%%%%%%%%%%\\     3    ','.|\n";
        std::cout << "|;____________/%%%%%Spicer%%%%%%\\____________;|\n";
    } else if (difficulty == 2){
        std::cout << "   ______________________________________________________\n";
        std::cout << "   [[]]-[[]]-[[]]-[[]]-[[]]-[[]]-[[]]-[[]]-[[]]-[[]]-[[]]\n";
        std::cout << "   .-.`| `-/-.__/.-'\\_.-._,'/`-._'\\_.-._`-'_/-._.'|/.-'\\-\n";
        std::cout << "   \\_.-`./`-._'\\__.-`-.__.-`--._/--.`-._\\`-._\\__.-)`-'._/\n";
        std::cout << "   `._-'.\\_.---._-.\\_`-..`\\_.---._`-.__.-`'._.--./`-'._,'\n";
        std::cout << "   __/`.-/    1  `.'_`./`.'   2   '.\\__.-`.'  3  (_.-\\_,-\n";
        std::cout << "    -./`.        -[[[[[[[[         [[[[[[[[        .'-,'/\n";
        std::cout << "   `\\,-/           |\\.-'|           |\\-'`|          ;\\_,-\n";
        std::cout << "   `._-/'          |._.-|           |.'`.|          _.`-_\n";
        std::cout << "    -./`.        -[[[[[[[[         [[[[[[[[        .'-,'/\n";
        std::cout << "   .-',`)          | /`.|           |`-/`|          .-'_/\n";
        std::cout << "   `\\,-/           |\\.-'|           |\\-'`|          ;\\_,-\n";
        std::cout << "    -./`._       -[[[[[[[[         [[[[[[[[        .'-,'/\n";
        std::cout << "    `.`--.~~-^_~-/.`-._`-.\\^~-_~-^/`-.'-,.'\\^~-~_^\"'`-.'_\n";
        std::cout << "    -,.'\"-\"~^-~_~- - - _- -~^-_.~ - -_ - - -~- . \"'\"-\"\"\"\n";
        std::cout << "    \"\"-\"'\"-\"\"-~- _~.^-~-^.-^_.- .^~.-  ~-. ~^_-\"\"-\"\"-\"'-\"\n";
        std::cout << "    jgs \"\"-\"'\"-\"    ~- ^. - ~ -~^ - ~  ^~- ~     \"\"-\"'-'\n";
    } else {
        std::cout << "                                                           |>>>\n";
        std::cout << "                   _                     _                 |\n";
        std::cout << "    ____________ .' '.   _____/----/-\\ .' './========\\    / \\\n";
        std::cout << "   //// ////// /V_.-._\\ |.-.-.|===| _ |-----| u    u |   /___\\\n";
        std::cout << "  // /// // ///==\\ u |. || | ||===||||| |1| |   ||   |  .| u |_ _ _ _ _ _\n";
        std::cout << " ///////-\\////====\\==|:::::::::::::::::::::::::::::::::::|u u| U U U U U\n";
        std::cout << " |----/\\u |--|++++|..|'''''''''''::::::::::::::''''''''''|+++|+-+-+-+-+-+\n";
        std::cout << " |u u|u | |u ||||||..|      2       '::::::::'       3   |===|>=== _ _ ==\n";
        std::cout << " |===|  |u|==|++++|==|              .::::::::.           | T |....| V |..\n";
        std::cout << " |u u|u | |u ||HH||         \\|/    .::::::::::.\n";
        std::cout << " |===|_.|u|_.|+HH+|_              .::::::::::::.              _\n";
        std::cout << "                __(_)___         .::::::::::::::.         ___(_)__\n";
        std::cout << "---------------/  / \\  /|       .:::::;;;:::;;:::.       |\\  / \\  \\-------\n";
        std::cout << "______________/_______/ |      .::::::;;:::::;;:::.      | \\_______\\________\n";
        std::cout << "|       |     [===  =] /|     .:::::;;;::::::;;;:::.     |\\ [==  = ]   |\n";
        std::cout << "|_______|_____[ = == ]/ |    .:::::;;;:::::::;;::::.     | \\[ ===  ]___|____\n";
        std::cout << "     |       |[  === ] /|   .:::::;;;::::::::;;;:::::.   |\\ [=  ===] |\n";
        std::cout << "_____|_______|[== = =]/ |  .:::::;;;::::::::::;;;:::::.  | \\[ ==  =]_|______\n";
        std::cout << " |       |    [ == = ] /| .::::::;;:::::::::::;;;::::::. |\\ [== == ]      |\n";
        std::cout << "_|_______|____[=  == ]/ |.::::::;;:::::::::::::;;;::::::.| \\[  === ]______|_\n";
        std::cout << "   |       |  [ === =] /.::::::;;::::::::::::::;;;:::::::.\\ [===  =]   |\n";
        std::cout << "___|_______|__[ == ==]/.::::::;;;:::::::::::::::;;;:::::::.\\[=  == ]___|_____\n";
    }
}


void Game::chooseCardFromBoostDeck() {
    // Vérification que le deck de boost existe et n'est pas vide
    if (m_boostingDeck.empty()) {
        std::cout << "Le deck de boost est vide." << std::endl;
        return;
    }

    std::vector<std::shared_ptr<Card>> selected_cards;
    std::random_device rd;   // Objet pour générer des nombres aléatoires
    std::mt19937 gen(rd());  // Générateur de nombres aléatoires Mersenne Twister
    std::uniform_int_distribution<> distrib(0, m_boostingDeck.size() - 1); // Distribution uniforme d'indices

    // Sélectionner trois cartes aléatoires différentes du deck de boost
    while (selected_cards.size() < 3) {
        int index = distrib(gen);
        auto card = m_boostingDeck[index];
        // Vérifier si la carte sélectionnée n'est pas déjà dans selected_cards
        if (std::find(selected_cards.begin(), selected_cards.end(), card) == selected_cards.end()) {
            selected_cards.push_back(card);
        }
    }

    // Largeur totale d'une carte dans l'affichage
    const int card_width = 22; // Largeur ajustée pour inclure les bordures

    std::cout << "+----------------------+  +----------------------+  +----------------------+" << std::endl;
    for (int i = 0; i < 3; ++i) {
        int manaCost = selected_cards[i]->getManaCost();
        if (manaCost > 9){
            cout << "|                 (" << selected_cards[i]->getManaCost() << ") |  ";
        } else {
            cout << "|                  (" << selected_cards[i]->getManaCost() << ") |  ";
        }
    }
    std::cout << "\n|                      |  |                      |  |                      |" << std::endl;
    // Affichage des noms des cartes centrés avec leur taille
    for (const auto& card : selected_cards) {
        std::string name = card->getName();
        int name_length = name.size();
        int padding_left = (card_width - name_length) / 2;
        int padding_right = card_width - name_length - padding_left;

        // Affichage du nom de la carte centré
        std::cout << "| " << std::string(padding_left, ' ') << name << std::string(padding_right - 1, ' ') << "|  ";
    }
    std::cout << "\n+----------------------+  +----------------------+  +----------------------+" << std::endl;

    // Collecte des lignes de description pour chaque carte
    std::vector<std::vector<std::string>> all_descriptions;
    size_t max_lines = 0;

    for (const auto& card : selected_cards) {
        string description = card->getDescription();
        size_t desc_length = description.size();
        std::vector<std::string> lines;

        size_t start = 0;
        while (start < desc_length) {
            size_t end = std::min(start + card_width - 2, desc_length); // Ajuster pour la bordure
            if (end < desc_length && description[end] != ' ') {
                size_t space_pos = description.rfind(' ', end);
                if (space_pos != std::string::npos && space_pos > start) {
                    end = space_pos;
                }
            }

            lines.push_back(description.substr(start, end - start));
            start = end + 1; // Skip the space
        }

        max_lines = std::max(max_lines, lines.size());
        all_descriptions.push_back(lines);
    }

    // Affichage des descriptions verticalement
    for (size_t i = 0; i < max_lines; ++i) {
        for (const auto& lines : all_descriptions) {
            if (i < lines.size()) {
                int line_length = lines[i].size();
                int padding_left = (card_width - 2 - line_length) / 2; // Ajuster pour la bordure
                int padding_right = card_width - 2 - line_length - padding_left;

                std::cout << "| " << std::string(padding_left, ' ') << lines[i] << std::string(padding_right, ' ') << " |  ";
            } else {
                std::cout << "| " << std::string(card_width - 2, ' ') << " |  "; // Ajuster pour la bordure
            }
        }
        std::cout << "\n";
    }
    std::cout << "+----------------------+  +----------------------+  +----------------------+" << std::endl;

    int choice;
    std::cout << "Choose a card to continue: (1 | 2 | 3)\n: ";
    std::cin >> choice;

    // Validation du choix du joueur
    while (true) {
        // Vérifier si l'entrée est un nombre entier
        if (std::cin.fail()) {
            std::cin.clear(); // Efface l'état de défaillance
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignore les caractères invalides
            std::cout << "Invalid input. Please enter a number between 1 and 3.\n: ";
        } else if (choice < 1 || choice > 3) {
            std::cout << "Invalid choice. Please choose 1, 2, or 3.\n: ";
        } else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignore les caractères restants
            break;
        }
        std::cin >> choice;
    }

    std::cout << "You chose card " << choice << ".\n";
    // Ajoutez ici la logique pour ce qui se passe après que l'utilisateur a fait un choix valide

    // Récupérer la carte choisie par le joueur à partir des cartes sélectionnées
    std::shared_ptr<Card> chosenCard = selected_cards[choice - 1];

    // Ajouter la carte choisie au deck du joueur
    m_player->addCardToBaseDeck(chosenCard);
    clearScreen();
    std::cout << "You chose: " << chosenCard->getName() << std::endl;
}

void Game::chooseArtefact() {
    // Vérifier si la liste d'artefacts est suffisante pour choisir 3 artefacts
    if (m_artifacts.size() < 3) {
        std::cerr << "Erreur : Nombre insuffisant d'artefacts dans la liste : " << m_artifacts.size() << std::endl;
        return;
    }

    // Sélection aléatoire de 3 indices distincts pour les artefacts
    std::vector<int> indices;
    while (indices.size() < 3) {
        int index = rand() % m_artifacts.size(); // Sélection aléatoire de l'indice
        if (std::find(indices.begin(), indices.end(), index) == indices.end()) {
            indices.push_back(index); // Ajout de l'indice s'il n'est pas déjà présent
        }
    }

    // Affichage des options d'artefacts disponibles
    std::cout << "Choose an artefact :" << std::endl;
    for (size_t i = 0; i < indices.size(); ++i) {
        auto artefact = m_artifacts[indices[i]];
        std::cout << i + 1 << ". " << artefact->getName() << "(" << artefact->getVisual() << ")" << " - " << artefact->getDescription() << std::endl;
        // Afficher d'autres détails si nécessaire, comme m_visual ou m_value pour les artefacts de type ShieldArtefact ou BonusShieldArtefact
    }

    // Demande à l'utilisateur de choisir un artefact
    int choix;
    std::cout << "Your Choice\n: ";
    std::cin >> choix;

    // Vérifier si le choix est valide
    if (choix >= 1 && choix <= static_cast<int>(indices.size())) {
        // Attribuer l'artefact choisi au joueur (par exemple, à travers m_player)
        m_player->addArtefact(m_artifacts[indices[choix - 1]]); // Supposant que m_player a une méthode addArtefact()
        std::cout << "You chose the artifact : " << m_artifacts[indices[choix - 1]]->getName() << std::endl;

        // Retirer l'artefact choisi de la liste d'artefacts
        m_artifacts.erase(m_artifacts.begin() + indices[choix - 1]);
    } else {
        std::cout << "Invalid Choice." << std::endl;
    }
}

