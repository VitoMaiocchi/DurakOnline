#include "drawable.hpp"
#include <Networking/message.hpp>

class LobbyNode : public TreeNode {
    public:
        LobbyNode();
        void updateExtends(Extends ext);
        Extends getCompactExtends(Extends ext);

        void handlePlayerUpdate(PlayerUpdate update);
        void handleAvailableActionUpdate(AvailableActionUpdate update);
    private:
        void callForAllChildren(std::function<void(std::unique_ptr<Node>&)> function);
        std::unique_ptr<Node> lobby;
};

class LoginScreenNode : public TreeNode {
    public:
        LoginScreenNode();
        void updateExtends(Extends ext);
        Extends getCompactExtends(Extends ext);

    private:
        void callForAllChildren(std::function<void(std::unique_ptr<Node>&)> function);
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
