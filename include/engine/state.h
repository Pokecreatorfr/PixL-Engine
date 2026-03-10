#pragma once

#include "engine/config.h"
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct State State;

typedef void (*StateEnterFn)(State*);
typedef void (*StateExitFn)(State*);
typedef void (*StateUpdateFn)(State*);

typedef enum {
    STATE_STEP_ENTER,
    STATE_STEP_UPDATE,
    STATE_STEP_EXIT
} StateStep;

struct State {
    const char* name;
    StateEnterFn enter;
    StateExitFn exit;
    StateUpdateFn update;
    void* userdata;
    StateStep step;
};

typedef struct {
    State* stack[STATE_STACK_MAX];
    int top;
} StateStack;

StateStack Engine_StateStack;

/**
 * @brief Initialize the state stack. Must be called before using any other state stack functions.
 *
 * @return true if initialization was successful, false otherwise.
 */
void StateStack_Init();
/**
 * @brief Get the current state on top of the stack.
 * 
 * @return State* 
 */
State* StateStack_Current();
/**
 * @brief  Get the current depth of the state stack.
 * 
 * @return uint8_t 
 */
uint8_t StateStack_GetDepth();
/**
 * @brief Push a state onto the stack. The state will be executed on the next call to StateStack_Execute().
 * 
 * @param state 
 * @return true if the state was successfully pushed, false if the stack is full.
 * @return false if the stack is full.
 */
bool StateStack_Push(State* state);
/**
 * @brief Pop the current state from the stack. The state's exit function will be called on the next call to StateStack_Execute().
 * 
 * @return true if the state was successfully popped, false if the stack is empty.
 * @return false if the stack is empty.
 */
bool StateStack_Pop();
/**
 * @brief Execute the current state on top of the stack. This will call the state's enter, update, or exit function depending on the current step.
 * 
 * @return 0 if the state was executed successfully, 1 if there is no state on the stack.
 */
uint8_t StateStack_Execute();
