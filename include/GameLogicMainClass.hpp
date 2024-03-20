#pragma once 
#include <GameLogicData.hpp>

void game_logic_main();

class GameLogicMainClass
{
    protected:
        GameLogicMainClass();
        static GameLogicMainClass* instance_;
    public:
        static GameLogicMainClass* GetInstance();

    private:
        GameLogicData* game_logic_data_;
};