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
};

class LoginScreenNode : public TreeNode {
    public:
        LoginScreenNode();
        void updateExtends(Extends ext);
        Extends getCompactExtends(Extends ext);

    private:
        void callForAllChildren(std::function<void(std::unique_ptr<Node>&)> function);
};

class GameOverScreen : public TreeNode {
    public:
        GameOverScreen(bool durak);
        void updateExtends(Extends ext);
        Extends getCompactExtends(Extends ext);

    private:
        void callForAllChildren(std::function<void(std::unique_ptr<Node>&)> function);
        const bool durak;
};