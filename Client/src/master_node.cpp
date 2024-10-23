#include "master_node.hpp"
#include "drawable.hpp"

#include <cassert>
#include <iostream>

bool master_node_exists = false;

std::shared_ptr<BufferNode> node;

MasterNode::MasterNode() {
    assert(!master_node_exists); //only one master node can exist
    master_node_exists = true;

    node = std::make_shared<BufferNode>();
    node->child = std::make_shared<ImageNode>("../cards/png/ace_of_spades.png");
    node->setBufferSize(BUFFERTYPE_ABSOLUTE, 50);
    node->child->setClickEventCallback([]() {
        std::cout << "Clicked on Ace of Spades" << std::endl;
    });
}

void MasterNode::callForAllChildren(std::function<void(std::shared_ptr<Node>)> function) {
    function(node);
}

void MasterNode::drawNew(Extends ext) {
    node->draw(ext);
}

uint MasterNode::minWidth(uint height) {return 0;}
uint MasterNode::minHeight(uint width) {return 0;}