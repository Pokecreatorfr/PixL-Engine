#include "engine/state.h"

void StateStack_Init()
{
    Engine_StateStack.top = -1;
}

State* StateStack_Current()
{
    if (Engine_StateStack.top < 0) {
        return NULL;
    }
    return Engine_StateStack.stack[Engine_StateStack.top];
}

uint8_t StateStack_GetDepth()
{
    return (uint8_t)(Engine_StateStack.top + 1);
}

bool StateStack_Push(State* state)
{
    if (Engine_StateStack.top >= STATE_STACK_MAX - 1) {
        return false;
    }
    state->step = STATE_STEP_ENTER;
    Engine_StateStack.stack[++Engine_StateStack.top] = state;
    return true;
}

bool StateStack_Pop()
{
    if (Engine_StateStack.top < 0) {
        return false;
    }
    Engine_StateStack.stack[Engine_StateStack.top--] = NULL;
    return true;
}

uint8_t StateStack_Execute()
{
    State* current = StateStack_Current();
    if (current == NULL) {
        return 1;
    }

    switch (current->step) {
    case STATE_STEP_ENTER:
        if (current->enter != NULL) {
            current->enter(current);
        }
        current->step = STATE_STEP_UPDATE;
        break;
    case STATE_STEP_UPDATE:
        if (current->update != NULL) {
            current->update(current);
        }
        break;
    case STATE_STEP_EXIT:
        if (current->exit != NULL) {
            current->exit(current);
        }
        StateStack_Pop();
        break;
    default:
        break;
    }
    return 0;
}