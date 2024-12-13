## Table of Contents

1. High Level Overview of the Code
   - Client
   - Networking
   - Server

2. [Git](#git)
   - Cloning the Repository
   - Before Starting Work: Pull Latest Changes
   - Creating a Branch for Your Work
   - Making Changes and Committing
   - Pushing Your Changes
   - Creating a Merge Request
   - Handling Merge Conflicts
   - Final Steps After Merging
3. [Durak](#durak)
   - Description
   - Rules
   - SRS Durak project first deadline
   - SDS Durak project second deadline
   - ToDo

---

# High level Overview of the Code
<details> <summary>Click to expand Code section</summary>

## Client
<details> <summary>Click to expand Client section</summary>
Text as found in Client/main.cpp.
Here is a rough overview over the high level functions in the DurakClient.

The DurakClient Code is made up of 3 main components.

1. The Node Framework
   (drawable.cpp)

2. The Global Game State including any child Nodes
   (gloabl_state.cpp, game_node.cpp toplevel_nodes.cpp) 

3. OpenGl this includes the Window and any actual rendering
   (opengl.cpp)

The Node Framework Provides a way to stucture anything displayed on screen
in a hierachy of nodes. Any updates in the Window size propagate trough the node tree.
The size of each Node in the tree is only calculated when the window size changes.
This Extends of the Node is used to handle drawing and mouse events like hover/click.
This makes everything scale dynamically.
(more detail in drawable.hpp)

The Global Game State handles all parts that have something to do with Durak Game Logic.
It handles and displays any updates from the server and provides things like a login and
game screen.
(more detail in global_state.hpp)

OpenGL is the graphics library used to render everything efficiently. To create a Window
glfw is used. opengl.cpp provided a way to render primitive things like Text, Images and Rectangles
to the screen each frame
(more detail in opengl.hpp)

</details>


## Networking
<details> <summary>Click to expand Networking section</summary>
Text as found in Networking/include/Networking/network.hpp.

### network_client.cpp, network_server.cpp
The networking cpps are responsible for providing all network interactions.
Each client opens 2 connections, one for receiving and one for sending. 
The messages are received on separate threads. Each receive thread adds 
any messages to a message queue. The newest message in the message queue
can be fetched by calling receive message.
The server version of receive Message blocks until a new message arrives.
The client version returns null if nothing is received to not block
rendering frames which happens on the same thread.

### message.cpp
All messages that are sent are of the abstact class Message.
The message class is responsible for serializing and deserializing any messages
The different Message types are derived from Message and have to implement
a "to"- and "from json" function
More details about the message types are in message.hpp, message.cpp.

</details>


## Server 
<details> <summary>Click to expand Server section</summary>
The Server is divided into four main Classes:
1. Server (Server/src/server.cpp) & MessageHandler (Server/src/msg_handler.cpp)
2. Game (Server/src/game.cpp)
3. Battle (Server/src/battle.cpp)
4. Card Manager (Server/src/card_manager.cpp)

### Server Class
The Server class is kept rather simple. It consists of a main loop to receive all incoming messages from the clients.
To simplify things we created a seperate function to handle all messages. So the server simply receives an incoming message and calls handleMessage(). Inside the message handler most of the high level server logic is implemented. It's basically just a big switch statement, that differentiates between the messages coming from the client and either passes them on accordingly to Game or directly handles them (connect events, name setting etc).

### Game Class
The Game class is constructed when all clients are ready and a new game is created. Inside the constructor of Game, the first Battle is set up and called (assigning first attacker, defining trump etc). Between individual Battles, Game will store information like Player Roles (Attacker, Defender, Co-Attacker, Idle, Finished) and will create new Battles after the last one has started. The functions probably used the most are handlePlayerCardEvent() and handlePlayerActionEvent(), which deconstruct and pass on card moves and button presses respectively, coming from the Client -> Server -> MessageHandler to Battle. It also checks if the game is over and will then return to Server.

### Battle Class
This class handles the bulk of the game logic. All messages that have to do with playing a card or pressing a button are evaluated here. To simplify stuff we have different BattlePhases and BattleTypes. BattlePhases aim to further divide a battle to provide modular code sections. For example if the defender has a card left to beat, the current BattlePhase is OPEN. We also need to differentiate between BattleTypes; First, Normal and Endgame where Endgame is when only two players remain and a move could end the game. The most general functions aim to differentiate what kind of Action or Move has to be handled and then call the according functions to make the move/action.

### Card Manager
This class handles all stuff related to cards. This includes Player Hands, the middle of the playing field (Battlefield), drawing cards and sending out card updates to the clients. It is a member of Game as we need only one deck per game.

</details>

</details>

# Git
<details> <summary>Click to expand Git section</summary>

To ensure smooth collaboration and avoid any repository mishaps, here's a clear Git workflow guide for our Durak project. Everyone works on their local machine, and we will use branches to isolate changes and avoid conflicts.

#### 1. **Cloning the Repository**

When you first start working on the project, you need to clone the repository to your local machine.

```bash
git clone https://gitlab.ethz.ch/durak/durak.git
cd durak
```

This will create a local copy of the repository. Make sure you navigate into the repository folder to start working.

---

#### 2. **Before Starting Work: Pull Latest Changes**

Before you start working each time, always pull the latest changes from the remote repository. This ensures your local copy is up to date with any changes made by others.

```bash
git pull origin main
```
This will fetch and merge any new changes into your local `main` branch.

---
> **Note:** While you can just use `git pull`, it defaults to pulling from the current branch's configured remote (usually `origin/main`). However, explicitly specifying `origin main` ensures you're pulling from the correct remote (`origin`) and branch (`main`). This is especially important when working with multiple remotes or branches, as it avoids accidental pulls from the wrong location.


---

#### 3. **Creating a Branch for Your Work**

Never work directly on the `main` branch. Instead, create a new branch for each feature or fix you're working on. This isolates your work from others.

```bash
git checkout -b feature/your-feature-name
```

Replace `your-feature-name` with a meaningful name related to your task, like `game-ui` or `bugfix-trump-rules`.

---

#### 4. **Making Changes and Committing**

Now, you're ready to make changes to the code. Once you're happy with your changes, stage the files and commit them.

1. Stage the files you've changed:

```bash
git add .
```

2. Commit your changes with a descriptive message:

```bash
git commit -m "Add new feature for card dealing logic"
```

Make sure your commit message is clear about what the changes are.

---

#### 5. **Pushing Your Changes**

After committing your changes, you need to push your branch to the remote repository.

```bash
git push origin feature/your-feature-name
```

This will upload your branch to the remote repository, making it available for others to review.

---
> **Note:** `origin` refers to the default name of the remote repository where your code is stored. It points to the repository URL you cloned from. We use `origin` to specify that we want to push or pull changes to/from this specific remote repository. It's important because it ensures you're working with the correct remote repository.
---



#### 6. **Creating a Merge Request**

Once your feature or fix is complete, create a Merge Request (MR) on GitLab to merge your branch into the `main` branch.

1. Go to the GitLab repository page.
2. Navigate to "Merge Requests."
3. Select "New Merge Request" and choose your feature branch as the source and `main` as the target.
4. Fill out the description and submit the MR.

Someone from the team will review your changes before they are merged into `main`.

---

#### 7. **Handling Merge Conflicts**

If there are merge conflicts when merging your branch into `main`, Git will prompt you to resolve them. You can see which files have conflicts with:

```bash
git status
```

After resolving the conflicts in the indicated files:

1. Stage the resolved files:
```bash
git add <filename>
```

2. Continue the merge process:

```bash
git commit
```
Finally, push the changes again:
```bash
git push origin feature/your-feature-name
```
---

#### 8. **Final Steps After Merging**

Once your changes are merged into the `main` branch, don't forget to update your local `main` branch to keep it in sync.

```bash
git checkout main git pull origin main
```

You can then delete your feature branch both locally and on the remote repository if you don't need it anymore.

- Delete the branch locally:

```bash
git branch -d feature/your-feature-name
```

- Delete the branch remotely:

```bash
git push origin --delete feature/your-feature-name
```
---

By following this workflow, we can collaborate effectively without running into issues like merge conflicts or overwriting each other's work.

</details>


# Durak
<details> <summary>Click to expand Durak section</summary>

## Description
Implementation of the game Durak in the curriculum of Software Engineering.
The rules might differ to the ones on [wikipedia](https://en.wikipedia.org/wiki/Durak).

## Rules 

<details> <summary>Click to expand Rules section</summary>

- **There is no winner, only one loser, the durak**
- You can only beat a card with another card of the same sign or trump.
- A trump card of any rank beats any non-trump card
- The ranking of the cards is as follows (ascending) **'2, 3, 4, 5, 6, 7, 8, 9, 10, J, Q, K, A.'**
- The game is counter-clockwise
- A battle is one turn
- An attack is a card that has been laid for the next player to defend
- A battle can consit of 6 small attacks (6 cards that are laid on the field).
- Only the cards that are already on the battle field can be added as attacks (the numbers must match).

##### Edge cases 
- The very first battle can only be a maximum of 5 attacks.
- The very first card cannot be passed on.
- 
</details>

## SRS Durak project first deadline 11.10

_link for the word document_: [word document](https://ethz-my.sharepoint.com/:w:/r/personal/dpoluyanov_ethz_ch/Documents/srs_durak.docx?d=w9ebff62a57a04528947512cd6dfb8ffc&csf=1&web=1&e=wYfiOv)

## SDS Durak project second deadline 25.10
_link for the word document_: [word document](https://ethz-my.sharepoint.com/:w:/r/personal/dpoluyanov_ethz_ch/Documents/sds_durak.docx?d=wee70dd9c8b994d2da4935af7b89b8c00&csf=1&web=1&e=7NLF7t)

_link for the instructions_: [Software Design](https://lec.inf.ethz.ch/se/2024_AS/project/book/phase2.html)

**_Hint:_** check out the lecture document for theory, especially the slides for Modeling and Specification: [lecture page](https://lec.inf.ethz.ch/se/2024_AS/)



## Server team
- Noah, Danil, Thomas

## Client team
- Vito, Eric

## Email team 
- Thomas
