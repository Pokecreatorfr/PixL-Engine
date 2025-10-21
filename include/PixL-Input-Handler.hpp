#pragma once

#include <SDL3/SDL.h>
#include <iostream>
#include <map>
#include <set>
#include <variant>
#include <vector>

class PixL_Engine;

enum InputType
{
    BOOLEAN,
    UINT8,
    UINT16,
    UINT32,
    UINT64,
    INT8,
    INT16,
    INT32,
    INT64,
    FLOAT,
    DOUBLE
};

struct InputBinding_Keyboard
{
    uint32_t id;
    InputType type;
    std::vector<SDL_Keycode> keys;
    std::variant<bool, uint8_t, uint16_t, uint32_t, uint64_t,
                 int8_t, int16_t, int32_t, int64_t,
                 float, double>
        value;
};

struct InputLayout
{
    std::vector<InputBinding_Keyboard> keyboardBindings;
    bool validate() const;
};

class PixL_Input_Handler
{
protected:
    PixL_Input_Handler();
    ~PixL_Input_Handler();
    static PixL_Input_Handler *_instance;

    friend class PixL_Engine;

public:
    static PixL_Input_Handler *
    getInstance()
    {
        if (!_instance)
        {
            _instance = new PixL_Input_Handler();
        }
        return _instance;
    }

    static void destroyInstance();

    bool use_layout(const InputLayout &layout);

    bool update(const std::set<SDL_Keycode> &keys);

    std::map<uint32_t, std::variant<bool, uint8_t, uint16_t, uint32_t, uint64_t,
                                    int8_t, int16_t, int32_t, int64_t,
                                    float, double>>
    getInputValues();

private:
    InputLayout currentLayout = {};
    std::map<uint32_t, std::variant<bool, uint8_t, uint16_t, uint32_t, uint64_t,
                                    int8_t, int16_t, int32_t, int64_t,
                                    float, double>>
        inputValues;
    bool layoutWarningIssued = false;
};
