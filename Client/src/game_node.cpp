#include "game_node.hpp"

//TODO 
//da chunt alles ine wo grendered wird während es game lauft
//endscreen und so nöd nur das mit de charte i de mitti und so

GameNode::GameNode() {
    //TDO
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