#include "Player.h"


using namespace std;

shared_ptr<Player> Enemy::m_player = nullptr;

void Player::innitTurn() {
    resetMana();
    flushHand();
    redoDeck();
    drawCardsFromDeck(5);
    playStartTurnArtefact();
    clearShield();
}


void Player::heal(int heal, int nbHeal) {
    cout << getName() << " heals for " << heal << " HP ";
    if (nbHeal > 1){
        cout << nbHeal << " times!";
    }
    cout << "\n";
    for (int i = 0; i < nbHeal; ++i) {
        m_currentHealth += heal;
        if(m_currentHealth > m_maxHealth){
            m_currentHealth = m_maxHealth;
        }
        break;
    }
}

void Player::takeDamage(int dmg) {
    if (m_shield > 0) {
        if (dmg <= m_shield) {
            m_shield -= dmg;
            return;
        } else {
            dmg -= m_shield;
            m_shield = 0;
        }
    }
    m_currentHealth -= dmg;
}

void Player::showHand() const {
    int cardCount = 0;
    const int maxCardsPerLine = 4; // Nombre maximum de cartes par ligne

    // Calculer la longueur totale de la bordure
    int borderLength = 0;
    for (const auto& card : m_hand) {
        cardCount++;
        borderLength += card->getName().length() + 9;
    }
    borderLength++;
    if (cardCount >= maxCardsPerLine) {
        borderLength = 120;
    }
    for (int j = 0; j < 120; ++j) {
        cout << "-";
    }
    // Afficher les informations de chaque carte sur plusieurs lignes
    for (int i = 0; i < m_hand.size(); ++i) {
        cout << "| " << m_hand[i]->getName() << " - (" << m_hand[i]->getManaCost() << ") ";

        // Si on a atteint le nombre maximum de cartes par ligne ou si c'est la dernière carte, passer à la ligne suivante
        if ((i + 1) % maxCardsPerLine == 0 || i == m_hand.size() - 1) {
            cout << "|\n";

            // Afficher la bordure intermédiaire si ce n'est pas la dernière ligne
            if (i != m_hand.size() - 1) {
                for (int j = 0; j < borderLength; ++j) {
                    cout << "-";
                }
            }
        }
    }

    // Afficher la bordure inférieure
    for (int i = 0; i < borderLength; ++i) {
        cout << "-";
    }
    cout << endl;
}

bool Player::isDefeated() const {
    return m_currentHealth <= 0;
}

// methods for all decks:

void Player::addCardToHand(const shared_ptr<Card>& card) {
    m_hand.push_back(card);
}



void Player::addCardToCombatDeck(const shared_ptr<Card>& card) {
    m_combatDeck.push_back(card);
}

void Player::innitCombatDeck() {
    m_combatDeck = m_baseDeck;
}

void Player::addCardToBaseDeck(const shared_ptr<Card>& card) {
    m_baseDeck.push_back(card);
}

void Player::addCardToGraveyard(const shared_ptr<Card>& card) {
    m_graveyard.push_back(card);
}

void Player::addCardToExile(const shared_ptr<Card>& card) {
    m_exileDeck.push_back(card);
}

void Player::addArtefact(shared_ptr<Artefact> artefact) {
    m_artefacts.push_back(artefact);
}

bool Player::emptyHand() const {
    return m_hand.empty();
}

bool Player::emptyDeck() const {
    return m_combatDeck.empty();
}

void Player::removeCardFromHand(const shared_ptr<Card>& card) {
    m_hand.erase(find(m_hand.begin(), m_hand.end(), card));
}

void Player::drawCardFromDeck() {
    // Si le deck est vide, récupère les cartes du cimetière
    if (m_combatDeck.empty()) {
        redoDeck();
    }

    // Vérifie encore si le deck est toujours vide après avoir potentiellement rechargé le deck
    if (m_combatDeck.empty()) {
        // Pas de cartes disponibles pour piocher
        return;
    }

    // Initialize random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, m_combatDeck.size() - 1);

    // Get a random index
    int randomIndex = dis(gen);

    // Get the card at the random index and add it to the hand
    std::shared_ptr<Card> drawnCard = m_combatDeck[randomIndex];
    m_hand.push_back(drawnCard);

    // Remove the card from the deck
    m_combatDeck.erase(m_combatDeck.begin() + randomIndex);
}

void Player::drawCardsFromDeck(int numCards) {
    for (int i = 0; i < numCards; ++i) {
        // Si le deck est vide, récupère les cartes du cimetière
        if (m_combatDeck.empty()) {
            redoDeck();
        }
        // Vérifie encore si le deck est toujours vide après avoir potentiellement rechargé le deck
        if (m_combatDeck.empty()) {
            // Pas de cartes disponibles pour piocher
            break;
        }
        drawCardFromDeck();
    }
}

void Player::redoDeck() {
    if (m_combatDeck.empty()) {
        if (!m_graveyard.empty()) {
            m_combatDeck = m_graveyard;
            m_graveyard.clear();
        }
    }
}

void Player::flushHand() {
    for (const auto& card : m_hand) {
        m_graveyard.push_back(card);
    }
    m_hand.clear();
}

void Player::playStartTurnArtefact() {
    for (shared_ptr<Artefact> artefact : m_artefacts) {
        artefact->triggerStartTurn();
    }
}

void Player::playAfterCardArtefact() {
    for (shared_ptr<Artefact> artefact : m_artefacts) {
        artefact->triggerAfterCard();
    }
}

void Player::display() const {
    // Affichage joueur
    unsigned long long space = ((120 - m_name.length()) / 2) - 4;
    for (int i = 0; i < space; ++i) {
        cout << " ";
    }
    cout << "---| " << m_name << " |---" << endl;

    // Affichage de la barre de vie
    int healthBarLength = 100; // Longueur totale de la barre de vie
    int filledLength = static_cast<int>((static_cast<double>(m_currentHealth) / m_maxHealth) * healthBarLength); // Longueur remplie en fonction du pourcentage de vie
    if (m_shield == 0){
        cout << "     (⛉";
    } else {
        cout << "   (⛨|" << m_shield;
    }
    cout << ")[";
    cout << setw(filledLength);
    cout << setfill('/') << left << "/";
    cout << setw(healthBarLength - filledLength);
    cout << setfill(' ') << right << "";
    cout << "] " << m_currentHealth << "/" << m_maxHealth << endl;
    // Affichage de la bordure inférieure
    for (int i = 0; i < 120; ++i) {
        cout << "-";
    }
}

void Player::playCard(shared_ptr<Card> card) {
    removeMana(card->getManaCost());
    removeCardFromHand(card);
    card->play();
}

void Player::addMana(int value) {
    m_currentMana += value;
}

void Player::removeMana(int value) {
    m_currentMana -= value;
}

void Player::addShield(int value) {
    m_shield += value;
}

void Player::removeShield(int value) {
    m_shield -= value;
}

void Player::clearShield(){
    m_shield = 0;
}

void Player::setArtefacts(shared_ptr<Enemy> enemy, Fight *fight) {
    if (!m_artefacts.empty())
    m_artefacts.begin()->get()->setEverything(shared_from_this(), enemy, fight);
}

void Player::displayArtefact(int index) {
    if (index < m_artefacts.size()){
        cout << m_artefacts[index]->getVisual();
    }
}


void clearScreen() {
    std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
}

void printTest(){
    std::cout << "Test\n";
}