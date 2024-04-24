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
        void update();
    private:
        GameLogicData* game_logic_data_ = GameLogicData::GetInstance();
        Camera* camera_ = Camera::GetInstance();

};