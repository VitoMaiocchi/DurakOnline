#include "drawable.hpp"
#include <Networking/message.hpp>

class LobbyNode : public TreeNode {
    public:
        LobbyNode();
        void updateExtends(Extends ext);
        Extends getCompactExtends(Extends ext);

        void handleAvailableActionUpdate(AvailableActionUpdate update);
    private:
        void callForAllChildren(std::function<void(std::unique_ptr<Node>&)> function);
        std::unique_ptr<Node> lobby;
        std::unique_ptr<Node> back_button;
        std::unique_ptr<Node> ready_button;
        std::unique_ptr<Node> settings_button;
};

class LoginScreenNode : public TreeNode {
    public:
        LoginScreenNode(Extends ext);
        void updateExtends(Extends ext);
        Extends getCompactExtends(Extends ext);

    private:
        void callForAllChildren(std::function<void(std::unique_ptr<Node>&)> function);
        std::unique_ptr<Node> placeholder_button;
};

class GameOverScreenNode : public TreeNode {
    public:
        GameOverScreenNode(bool durak= 1);
        void updateExtends(Extends ext);
        Extends getCompactExtends(Extends ext);

    private:
        void callForAllChildren(std::function<void(std::unique_ptr<Node>&)> function);
        const bool durak;
};
