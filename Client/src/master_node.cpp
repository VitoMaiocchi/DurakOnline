#include "master_node.hpp"
#include "drawable.hpp"

#include <cassert>
#include <iostream>
#include <Networking/util.hpp>

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