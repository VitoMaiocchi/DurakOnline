#include "drawable.hpp"
#include <Networking/message.hpp>

class LobbyNode : public TreeNode {
    public:
        void draw();
        LobbyNode(Extends ext);
        void updateExtends(Extends ext);
        Extends getCompactExtends(Extends ext);
        void playerUpdateNotify();
        void handleReadyUpdate(ReadyUpdate update);
    private:
        void callForAllChildren(std::function<void(std::unique_ptr<Node>&)> function);
        std::unique_ptr<Node> lobby;
        std::unique_ptr<Node> setting;
        std::unique_ptr<Node> back_button;
        std::unique_ptr<Node> ready_button;
        std::unique_ptr<Node> settings_button;
        std::list<std::unique_ptr<Node>> player_nodes;
};

class LoginScreenNode : public Node {
    public:
        void draw();
        LoginScreenNode(Extends ext);
        void updateExtends(Extends ext);
        Extends getCompactExtends(Extends ext);
        void connect();

    private:
        std::string name;
        std::string ip;
        void callForAllChildren(std::function<void(std::unique_ptr<Node>&)> function);
        std::unique_ptr<Node> connect_button;
        std::unique_ptr<Node> name_input;
        std::unique_ptr<Node> ip_input;
};

class GameOverScreenNode : public Node {
    public:
        void draw();
        GameOverScreenNode(Extends ext, bool durak=1);
        void updateExtends(Extends ext);
        Extends getCompactExtends(Extends ext);

    private:
    
        void callForAllChildren(std::function<void(std::unique_ptr<Node>&)> function);
        const bool durak;
        std::unique_ptr<Node> back_button;
        std::unique_ptr<Node> rematch_button;
};
