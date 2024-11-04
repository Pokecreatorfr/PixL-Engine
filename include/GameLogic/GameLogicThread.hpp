#pragma once

#include <GameLogic/GameLogicHandler.hpp>
#include <SystemEvent.hpp>
#include <mutex>

void GameLogicThread(SystemEvent *event, std::mutex *mutex)
{
    GameLogicHandler *gameLogicHandler = new GameLogicHandler(event);
    while (!event->isQuit())
    {
        mutex->lock();
        gameLogicHandler->update();
        mutex->unlock();
    }
}