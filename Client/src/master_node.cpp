#include "master_node.hpp"
#include "drawable.hpp"

#include <cassert>
#include <iostream>
#include <Networking/util.hpp>
#include <Networking/message.hpp>

bool master_node_exists = false;

std::shared_ptr<BufferNode> node;
std::shared_ptr<BufferNode> node2;
std::shared_ptr<RectangleNode> rect_node;
std::shared_ptr<RectangleNode> rect_node2;

MasterNode::MasterNode() {
    assert(!master_node_exists); //only one master node can exist
    master_node_exists = true;

    node = std::make_shared<BufferNode>();
    node->child = std::make_shared<ImageNode>(Card(RANK_ACE, SUIT_SPADES).getFileName());
    node->setBufferSize(BUFFERTYPE_RELATIVE, 50);
    node->child->setClickEventCallback([]() {
        std::cout << "Clicked on Ace of Spades" << std::endl;
    });

    node2 = std::make_shared<BufferNode>();
    node2->child = std::make_shared<ImageNode>(Card(RANK_QUEEN, SUIT_DIAMONDS).getFileName());
    node2->setBufferSize(BUFFERTYPE_ABSOLUTE, 50);
    node2->child->setClickEventCallback([]() {
        std::cout << "Clicked on Queen of Diamonds" << std::endl;
    });

    rect_node = std::make_shared<RectangleNode>(0, 0, 0);
    rect_node2 = std::make_shared<RectangleNode>(1.0, 0, 0);
}

void MasterNode::callForAllChildren(std::function<void(std::shared_ptr<Node>)> function) {
    function(rect_node);
    function(rect_node2);
    function(node);
    function(node2);
}

void MasterNode::updateExtends(Extends ext) {
    extends = ext;
    Extends ext1 = {ext.x, ext.y, ext.width/2, ext.height};
    Extends ext2 = {ext.x+ext.width/2, ext.y, ext.width/2, ext.height};
    rect_node->updateExtends(ext1);
    rect_node2->updateExtends(ext2);
    node->updateExtends(ext1);
    node2->updateExtends(ext2);
}

Extends MasterNode::getCompactExtends(Extends ext) {
    return ext;
}

void handleMessage(std::unique_ptr<Message> message) {
    //sobald message.hpp fertig isch chömmer da je nach message handle 
    //oder zu game_node witer ge
    switch (message->messageType) {
        case MESSAGETYPE_ILLEGAL_MOVE_NOTIFY:
            //erst mal nüt
        break;
        case MESSAGETYPE_CARD_UPDATE:
            //gaht zu game
        break;
        case MESSAGETYPE_PLAYER_UPDATE:
            //bö ka
        break;
        case MESSAGETYPE_BATTLE_STATE_UPDATE:
            //gaht zu game
        break;
        case MESSAGETYPE_AVAILABLE_ACTION_UPDATE:
            //gaht zu game falls grad nöd lobby isch
        break;
        case MESSAGETYPE_GAME_STATE_UPDATE:
            //wird da ghandled
        break;
        default:
            //print debug warning: unknown message type
        break;
    }
}