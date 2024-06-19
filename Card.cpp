#include "Player.h"



using namespace std;

shared_ptr<Player> Card::m_player = nullptr;
shared_ptr<Enemy> Card::m_enemy = nullptr;
Fight* Card::m_fight = nullptr;


// Card class implementation

void Card::play() {
    cout << "Playing card: " << m_name;
    cout << "\n";
}

void Card::remove() {
    if (isUniqueUse()){
        m_player->addCardToExile(shared_from_this());
    }else{
        m_player->addCardToGraveyard(shared_from_this());
    }
}

void CombatCard::play() {
    Card::play();
    for (int i = 0; i < m_numAttacks; ++i) {
        int totalDamage = m_player->calculateDamage(m_damage);
        m_enemy->takeDamage(totalDamage);
        m_player->removeWeakness(1);
    }
    Card::remove();
}

string CombatCard::getDescription() {
    int totalDamage = m_player->calculateDamage(m_damage);
    string result = "This card does " + to_string(totalDamage) + " damages";
    if (m_numAttacks > 1){
        result += " " + to_string(m_numAttacks) + " times";
    }
    return result;
}



void SkillCard::play() {
    Card::play();
}

void SkillCard_SimpleHeal::play() {
    SkillCard::play();
    m_player->heal(m_healValue, m_numHeals);
    Card::remove();
}

string SkillCard_SimpleHeal::getDescription() {
    string result = "This card Heal you for " + to_string(m_healValue);
    if (m_numHeals > 1){
        result += " " + to_string(m_numHeals) + " times";
    }
    return result;
}

void SkillCard_SimpleShield::play() {
    SkillCard::play();
    m_player->addShield(m_value);
    Card::remove();
}

void PassiveCard::play() {
    if (!m_played) {
        Card::play();
        m_played = true;
        // Actions générales quand une carte passive est jouée pour la première fois
        m_fight->addToPassivDeck(shared_from_this());  // Appel à la méthode spécifique de PassiveCard
    } else {
        // La carte a déjà été jouée, appeler la méthode spécifique de la sous-classe
        doPassiveEffect();
    }
}

void PassiveDamageCard::doPassiveEffect() {
    if (m_enemy) {
        m_enemy->takeDamage(m_damage);
    }
}