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
        bool first_battle_ = false;;

    public:
        Battle(); //default Constructor
        Battle(bool first_battle) : first_battle_(first_battle){
            if(first_battle_ == true){
                max_attacks = 5;
            }
            else{
                max_attacks = 6;
            }
        }
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