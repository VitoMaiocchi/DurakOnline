#ifndef BATTLE_HPP
#define BATTLE_HPP

#include <vector>
#include <tuple>

class Battle {
    private:
        //we might not need that because the other vector saves it already
        std::vector<int> players; //saves the player ids of the players
        std::vector<std::tuple<int, int>> battle_state; //attacking, defending, spectating
        bool defending_flag;
        int max_attacks;
        int attacks_to_defend; 

    public:
        Battle(); //default Constructor
        Battle(int max_attacks_) : max_attacks(max_attacks_){}
        ~Battle(); //default Destructor
        

        bool handleCardEvent();
        bool handleActionEvent();
        bool successfulDefend();
        bool passOn();
        bool isValidMove();

        // helper functions
        void attack(); 
        void defend();
};

#endif
//first_battle