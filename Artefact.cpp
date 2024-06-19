//
// Created by lucas on 16/06/2024.
//

#include "Artefact.h"

std::shared_ptr<Player> Artefact::m_player = nullptr;
std::shared_ptr<Enemy> Artefact::m_enemy = nullptr;
Fight* Artefact::m_fight = nullptr;

void BonusShieldArtefact::triggerStartTurn() {
    if (m_fight->getTurn() < 2){
        m_player->addShield(m_value);
    }
}