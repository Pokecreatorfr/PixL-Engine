#include <GameLogicMainClass.hpp>

GameLogicMainClass::GameLogicMainClass()
{
}

GameLogicMainClass* GameLogicMainClass::instance_ = nullptr;

GameLogicMainClass* GameLogicMainClass::GetInstance()
{
    if (instance_ == nullptr)
    {
        instance_ = new GameLogicMainClass();
    }
    return instance_;
}


void game_logic_main()
{
    GameLogicMainClass* game_logic = GameLogicMainClass::GetInstance();
    GameLogicData* game_logic_data = GameLogicData::GetInstance();
}
