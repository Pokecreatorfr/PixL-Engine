#include <GameLogicData.hpp>

GameLogicData::GameLogicData()
{
}

GameLogicData* GameLogicData::instance_ = nullptr;

GameLogicData* GameLogicData::GetInstance()
{
    if (instance_ == nullptr)
    {
        instance_ = new GameLogicData();
    }
    return instance_;
}

std::vector<int> GameLogicData::GetPressedKeys()
{
    return this->pressed_keys;
}

void GameLogicData::SetPressedKeys(std::vector<int> keys)
{
    this->pressed_keys = keys;
}

bool GameLogicData::GetQuit()
{
    return this->quit;
}

void GameLogicData::Quit()
{
    this->quit = true;
}

GameLogicState GameLogicData::GetState()
{
    return this->state;
}

void GameLogicData::SetState(GameLogicState state)
{
    this->state = state;
}