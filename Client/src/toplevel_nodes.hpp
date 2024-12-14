#include "drawable.hpp"
#include <Networking/message.hpp>

/*
### LobbyNode ###
The `LobbyNode` class represents the lobby interface where players can prepare before starting a game. 
It extends the `TreeNode` class, inheriting functionality for managing and drawing child nodes. 
It provides specialized features for handling player readiness and updating the lobby's display.

- **Functionality**:
  - Draws the lobby screen, including players and buttons.
  - Updates the layout and size of the node when the window size changes.
  - Notifies the lobby of updates to player states.
  - Handles readiness updates via a `ReadyUpdate`.

- **Specialized Child Nodes**:
  - `lobby`: Main lobby display.
  - `setting`: Lobby settings display.
  - `back_button`: Button to return to the previous screen.
  - `ready_button`: Button to toggle readiness.
  - `settings_button`: Button to access lobby settings.
  - `player_nodes`: List of nodes representing players.

- **Methods**:
  - `draw()`: Draws the lobby screen and all child nodes.
  - `updateExtends(Extends ext)`: Updates the layout and size of the lobby node.
  - `getCompactExtends(Extends ext)`: Calculates and returns the minimum bounds required for the node.
  - `playerUpdateNotify()`: Notifies the lobby of changes in player data.
  - `handleReadyUpdate(ReadyUpdate update)`: Handles updates to the readiness state of players.
  - `callForAllChildren(std::function<void(std::unique_ptr<Node>&)> function)`: Iterates over all child nodes to apply a given function.

---

### LoginScreenNode ###
The `LoginScreenNode` class represents the login screen interface where players can input their name and IP address to connect to a server. 
It extends the base `Node` class and includes input handling and connection functionality.

- **Functionality**:
  - Displays input fields for the player's name and server IP.
  - Provides a button to attempt connection to the server.

- **Specialized Child Nodes**:
  - `connect_button`: Button to connect to the server.
  - `name_input`: Input field for the player's name.
  - `ip_input`: Input field for the server's IP address.

- **Methods**:
  - `draw()`: Draws the login screen and all input elements.
  - `updateExtends(Extends ext)`: Updates the layout and size of the node.
  - `getCompactExtends(Extends ext)`: Calculates and returns the minimum bounds required for the node.
  - `connect()`: Attempts to connect to the server using the provided name and IP.
  - `callForAllChildren(std::function<void(std::unique_ptr<Node>&)> function)`: Iterates over all child nodes to apply a given function.

---

GameOverScreenNode
The GameOverScreenNode class represents the game over screen, where players can either return to the lobby or quit. 
It extends the base Node class and includes functionality for post-game options. 

- Functionality: 
    - Displays game over options, including buttons to return to the lobby or quit. 
    - Shows a different screen and text, depeding if you are the durak or not. 
    
- Specialized Child Nodes: 
    - lobby_button: Button to return to the lobby. 
    - quit_button: Button to quit 

- Methods: 
    - draw(): Draws the game over screen and all child nodes. 
    - updateExtends(Extends ext): Updates the layout and size of the node. 
    - getCompactExtends(Extends ext): Calculates and returns the minimum bounds required for the node. 
    - callForAllChildren(std::function<void(std::unique_ptr<Node>&)> function): Iterates over all child nodes to apply a given function. 

- Special Attributes: 
    - durak: Boolean indicating whether the player was the "durak". 

*/

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
        const bool durak;

    private:
        void callForAllChildren(std::function<void(std::unique_ptr<Node>&)> function);
        std::unique_ptr<Node> lobby_button;
        std::unique_ptr<Node> quit_button;
};
