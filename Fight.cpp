#include "Fight.h"

// Initialisation du combat avec le joueur et l'ennemi
Fight::Fight(std::shared_ptr<Player> player, std::shared_ptr<Enemy> enemy)
        : m_player(player), m_enemy(enemy), m_turn(0) {
}

// Destructeur
Fight::~Fight() {
    clearPassivDeck();
}

void Fight::innit() {
    cout << m_player->getName() << " is fighting against " << m_enemy->getName() << endl;
    m_player->setArtefacts(m_enemy, this);
    m_player->innitCombatDeck();
    m_player->setEnemy(m_enemy);
    m_player->setFight(this);
    clearScreen();
}

// Démarrer le combat
void Fight::play() {
    innit();
    while (!m_player->isDefeated() && !m_enemy->isDefeated()) {
        m_enemy->innitTurn();
        m_turn++;
        cout << m_turn << endl;
        playerTurn();
        clearScreen();
        // Tour de l'ennemi
        enemyTurn();
    }
    endFight();
}

// Terminer le combat
void Fight::endFight() {
    if (m_player->isDefeated()) {
        cout << m_player->getName() << " has been defeated by " << m_enemy->getName() << "." << endl;
    } else if (m_enemy->isDefeated()) {
        cout << m_player->getName() << " has defeated " << m_enemy->getName() << "!" << endl;
        m_player->clearExile();
        m_player->clearGraveyard();
        m_player->flushHand();
        clearPassivDeck();
    }
}

// Tour du joueur
void Fight::playerTurn() {
// Vider la main du joueur et piocher de nouvelles cartes
    m_player->innitTurn();
    playPassivs();
    m_player->playStartTurnArtefact();

    // Réinitialiser les choix du joueur
    char choice = ' ';
    char playChoice = ' ';

    // Boucle pour que le joueur choisisse et joue une carte ou passe son tour
    while (choice != 'p' && !m_player->isDefeated() && !m_enemy->isDefeated()) {
        displayFightInterface();

        // Demander au joueur de choisir une carte à jouer ou de passer
        cout << "Choose the number of the card you want to play, or pass with 'p'\n: ";
        cin >> choice;

        // Valider le choix du joueur
        while (choice != 'p' && (choice < '1' || choice > '0' + m_player->getHandSize())) {
            cout << "Invalid choice. Choose again!\n: ";
            cin >> choice;
        }

        if (choice != 'p') {
            int cardNum = choice - '0' - 1;

            // Obtenir un pointeur vers la carte sélectionnée dans la main du joueur
            shared_ptr<Card> currentCard = m_player->getCardFromHand(cardNum);

            // Vérifier si le joueur a assez de mana pour jouer cette carte
            if (m_player->getCurrentMana() < currentCard->getManaCost()) {
                clearScreen();
                cout << "Not enough mana to play this card!" << endl;
                continue; // Demander un autre choix de carte ou de passer
            }

            // Demander au joueur s'il veut jouer cette carte
            do {
                // Afficher les détails de la carte sélectionnée
                clearScreen();
                displayFightInterface(currentCard);
                cout << "Do you want to play this card? (Y/N)\n: ";
                cin >> playChoice;
            } while (playChoice != 'Y' && playChoice != 'y' && playChoice != 'N' && playChoice != 'n');

            if (playChoice == 'y' || playChoice == 'Y') {
                // Jouer la carte et la retirer de la main du joueur
                clearScreen();m_player->playCard(currentCard);
                m_player->redoDeck();
                m_player->playAfterCardArtefact();
                // Vérifier si le combat est terminé
                if (m_enemy->isDefeated() || m_player->isDefeated()) {
                    break;
                }
            }
        }
    }
}

// Tour de l'ennemi
void Fight::enemyTurn() {
    m_enemy->clearShield();
    if (!m_enemy->isDefeated()) {
        m_enemy->playTurn();
    }
}

// Afficher l'interface de combat
void Fight::displayFightInterface(shared_ptr<Card> chosenCard) {
    m_enemy->display();
    int cardWidth = 40; // Largeur de la carte
    cout << "\n";
    // Message à centrer
    std::string message = "!!! " + m_enemy->AttackMessage() + " !!!";

    // Calculer les espaces nécessaires de chaque côté du message
    int spaces = (120 - message.length() - 1) / 2;

    // Afficher le message centré
    if (spaces > 0) {
        std::string padding(spaces, ' '); // Créer une chaîne de caractères avec 'spaces' espaces
        std::cout << padding << message << std::endl;
    } else {
        std::cout << message << std::endl; // Si le message est plus long que la largeur du terminal, l'afficher normalement
    }

    // Bord supérieur de la carte
    cout << string(40, ' ') << string(cardWidth, '-') << endl;

    // Contenu de la carte - Nom et coût en mana
    cout << string(40, ' ') << "| " << string((cardWidth - chosenCard->getName().size() - 8) / 2, ' ') << chosenCard->getName() << " (" << chosenCard->getManaCost() << ") " << string((cardWidth - chosenCard->getName().size() - 8) / 2 + (cardWidth - chosenCard->getName().size() - 8) % 2, ' ') << "|" <<  endl;

    // Ligne de séparation
    cout << string(40, ' ') << string(cardWidth, '-') << endl;

    // Affichage de la description centrée
    string description = chosenCard->getDescription();
    int descriptionLength = description.size();
    int paddingSize = (cardWidth - descriptionLength - 4) / 2;

    cout << string(40, ' ') << "| ";
    if (paddingSize > 0) {
        cout << string(paddingSize, ' ') << description << string(cardWidth - descriptionLength - paddingSize - 4, ' ');
    } else {
        for (int i = cardWidth - 4; i > 0; --i) {
            if(description[i] == ' '){
                cout << description.substr(0, i);
                for (int j = i; j < cardWidth - 4; ++j) {
                    cout << " ";
                }
                break;
            }
        }
    }
    cout << " |" << endl;

    // Si la description est plus longue que la largeur de la carte, afficher la suite sur la ligne suivante
    if (descriptionLength > cardWidth - 4) {
        cout << string(40, ' ') << "| ";
        cout << description.substr(cardWidth - 4) << string(cardWidth - 4 - description.substr(cardWidth - 4).size(), ' ') << " |" << endl;
    }

    // Affichage de l'info "unique" si applicable
    cout << string(40, ' ') << "| ";
    if(chosenCard->isUniqueUse()) {
        cout << "               unique               "; // ce serait bien en rouge
    } else {
        cout << "                                    ";
    }
    cout << " |" << endl;

    // Bord inférieur de la carte
    cout << string(40, ' ') << string(cardWidth, '-') << endl;

    // Lignes vides en dessous de la carte
    for (int i = 0; i < 4; ++i) {
        cout << endl;
    }

    m_player->display();
    m_player->showHand();
}

void Fight::displayFightInterface() {
    // Affichage de l'ennemi
    m_enemy->display();

    // Lignes vides au-dessus de la carte
    for (int i = 0; i < 17; ++i) {
        m_player->displayArtefact(i);
        if (i == 1){
            std::string message = "!!!" + m_enemy->AttackMessage() + "!!!"; // le message à centrer
            int lineLength = 120; // longueur de la ligne du terminal
            int messageLength = message.length(); // longueur du message
            int spaces = (lineLength - messageLength) / 2; // calcul des espaces nécessaires de chaque côté

            if (spaces > 0) {
                std::string padding(spaces, ' '); // créer une chaîne de caractères avec 'spaces' espaces
                std::cout << padding << message << std::endl; // afficher les espaces suivis du message
            } else {
                std::cout << message << std::endl; // si le message est plus long que la ligne, afficher le message sans padding
            }        }
        std::cout << "\n";
    }
    m_enemy->show();

    // Affichage du joueur et de sa main
    m_player->display();
    m_player->showHand();
}

// Obtenir le numéro du tour actuel
int Fight::getTurn() const {
    return m_turn;
}

void Fight::playPassivs() {
    for (auto& card : m_passiveDeck) {
        card->play();
    }
}

void Fight::clearPassivDeck() {
    m_passiveDeck.clear();
}

void Fight::addToPassivDeck(shared_ptr<Card> card) {
    m_passiveDeck.push_back(card);
}

void Fight::removeToPassivDeck(shared_ptr<Card> card) {
    auto it = std::find(m_passiveDeck.begin(), m_passiveDeck.end(), card);
    if (it != m_passiveDeck.end()) {
        m_passiveDeck.erase(it);
    }
}
