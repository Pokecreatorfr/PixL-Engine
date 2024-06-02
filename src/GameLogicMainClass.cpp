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

void GameLogicMainClass::update()
{
    switch (game_logic_data_->GetState())
    {
        case GameLogicState::TITLE_SCREEN:
            break;
        case GameLogicState::MAIN_MENU:
            break;
        case GameLogicState::OVERWORLD:
            for(int i = 0; i < game_logic_data_->GetPressedKeys().size(); i++)
            {
                int key = game_logic_data_->GetPressedKeys()[i];
                if (key == game_logic_data_->GetSave()->GetKeys()[ActionKey::UP] )
                {
                    *camera_->Getposy() -= 32;
                }
                if (key == game_logic_data_->GetSave()->GetKeys()[ActionKey::DOWN] )
                {
                    *camera_->Getposy() += 32;
                }
                if (key == game_logic_data_->GetSave()->GetKeys()[ActionKey::LEFT] )
                {
                    *camera_->Getposx()  -= 32;
                }
                if (key == game_logic_data_->GetSave()->GetKeys()[ActionKey::RIGHT] )
                {
                    *camera_->Getposx() += 32;
                }
            }
            break;
        case GameLogicState::MENU:
            break;
        case GameLogicState::PAUSE:
            break;
    }
}
