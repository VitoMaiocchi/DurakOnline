#include "game_node.hpp"
#include "master_node.hpp"

//TODO 
//da chunt alles ine wo grendered wird während es game lauft
//endscreen und so nöd nur das mit de charte i de mitti und so

GameNode::GameNode() {
    //TODO
}

void GameNode::updateExtends(Extends ext) {
    //TODO
}

void GameNode::callForAllChildren(std::function<void(std::shared_ptr<Node>)> function) {
    //TODO
}

Extends GameNode::getCompactExtends(Extends ext) {
    return ext;
}

void GameNode::handleCardUpdate(CardUpdate update) {
    //TODO
}

void GameNode::handleBattleStateUpdate(BattleStateUpdate update) {
    //TODO
}

void GameNode::handleAvailableActionUpdate(AvailableActionUpdate update) {
    //TODO
}

void GameNode::playerUpdateNotify() {
    //TODO
    GlobalState::players;
}