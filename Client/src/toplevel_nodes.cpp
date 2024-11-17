#include "toplevel_nodes.hpp"

//LobbyNode

class Lobby : public LeafNode {
    
    private:
    std::list<Player> players;
    std::unique_ptr<BufferNode> player_buffer;

    std::unique_ptr<LinearStackNode> player_name_stack;
    std::unique_ptr<LinearStackNode> horizontal_stack;

    std::unique_ptr<RectangleNode> base_rectangle;
    std::unique_ptr<BufferNode> base_buffer; 

    std::unique_ptr<TextNode> lobby_title;
    std::unique_ptr<BufferNode> lobby_title_buffer;

    public:
    Extends getCompactExtends(Extends ext) {return ext;}

    Lobby() {
        //create horizontal stack
        horizontal_stack = std::make_unique<LinearStackNode>();
        horizontal_stack->setStackType(STACKDIRECTION_HORIZONTAL, STACKTYPE_COMPACT);
        //Create player_name stack
        player_name_stack = std::make_unique<LinearStackNode>();
        player_name_stack->setStackType(STACKDIRECTION_VERTICAL, STACKTYPE_COMPACT);
        //Create and set buffer
        base_buffer = std::make_unique<BufferNode>();
        base_buffer->setBufferSize(BUFFERTYPE_ABSOLUTE, 100);
        lobby_title_buffer = std::make_unique<BufferNode>();
        lobby_title_buffer->setBufferSize(BUFFERTYPE_ABSOLUTE, 200);
        //Create Objects
        base_rectangle = std::make_unique<RectangleNode>(1.0f, 1.0f, 1.0f);
        lobby_title = std::make_unique<TextNode>(" LOBBY ",0.0f, 0.0f, 0.0f);

        player_name_stack->children.push_back(std::make_unique<TextNode>(" YOU ",0.0f, 0.0f, 0.0f));
        player_name_stack->children.push_back(std::make_unique<ImageNode>("../Client/resources/cards/2_of_clubs.png"));

        horizontal_stack->children.push_back(std::move(player_name_stack));
        
        // Set the rectangle as a child of the buffer
        base_buffer->child = std::move(base_rectangle);
        lobby_title_buffer->child = std::move(lobby_title);
    }

    void draw() override {
        // Define the base extents for the entire node
        Extends base_ext = {
            extends.x,
            extends.y,
            extends.width,
            extends.height,
            0
        };
        
        Extends title_ext = {
            extends.x,
            extends.y+(extends.height/2),
            extends.width,
            extends.height/2,
            0
        };

        Extends name_ext = {
            extends.x,
            extends.y,
            extends.width,
            extends.height/2,
            0
        };

        // Update the extents of the buffer node with the base extents
        base_buffer->updateExtends(base_ext);
        lobby_title_buffer->updateExtends(title_ext);
        horizontal_stack->updateExtends(name_ext);
        //vertical_stack->updateExtends(name_ext);

        // Draw the buffered rectangle
        base_buffer->draw();
        lobby_title_buffer->draw();
        horizontal_stack->draw();
        //vertical_stack->draw();
    }

    void updatePlayer(std::list<Player> players){

    }
    
};

LobbyNode::LobbyNode(){
    lobby = std::make_unique<Lobby>();
}

void LobbyNode::updateExtends(Extends ext){
    extends = ext;

    
    lobby->updateExtends(ext);
}
        
Extends LobbyNode::getCompactExtends(Extends ext){
    return ext;
}

void LobbyNode::handlePlayerUpdate(PlayerUpdate update){
    
}

void LobbyNode::handleAvailableActionUpdate(AvailableActionUpdate update){

}

void LobbyNode::callForAllChildren(std::function<void(std::unique_ptr<Node>&)> function){
    function(lobby);
}

//LoginScreenNode 
class LoginScreen : public LeafNode {
    private:

    public:
    Extends getCompactExtends(Extends ext) {return ext;}

};

void LoginScreenNode::callForAllChildren(std::function<void(std::unique_ptr<Node>&)> function){

}

//GameOverScreen
class GameOverScreen : public LeafNode {
    private:

    public:
    Extends getCompactExtends(Extends ext) {return ext;}

};

void GameOverScreenNode::callForAllChildren(std::function<void(std::unique_ptr<Node>&)> function){

}

void LoginScreenNode::updateExtends(Extends ext) {
    extends = ext;
}

Extends LoginScreenNode::getCompactExtends(Extends ext) {
    return ext;
}

void GameOverScreenNode::updateExtends(Extends ext) {
    extends = ext;
}

Extends GameOverScreenNode::getCompactExtends(Extends ext) {
    return ext;
}