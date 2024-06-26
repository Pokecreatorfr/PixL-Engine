#pragma once
#include <vector>
#include <Camera.hpp>
#include <Save.hpp>

enum class GameLogicState
{
    TITLE_SCREEN,
    MAIN_MENU,
    OVERWORLD,
    MENU,
    PAUSE,
};

class GameLogicData
{
    protected:
        GameLogicData();
        static GameLogicData* instance_;
    public:
        static GameLogicData* GetInstance();
        std::vector<int> GetPressedKeys();
        void SetPressedKeys(std::vector<int> keys);
        bool GetQuit();
        void Quit();
        GameLogicState GetState();
        void SetState(GameLogicState state);
        Save* GetSave();

    private:
        std::vector<int> pressed_keys;
        bool quit = false;
        GameLogicState state = GameLogicState::OVERWORLD;
        Save* save_ = Save::GetInstance();
};