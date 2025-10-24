#include <PixL-Input-Handler.hpp>

#include <algorithm>

namespace
{
using InputVariant = std::variant<bool, uint8_t, uint16_t, uint32_t, uint64_t,
                                  int8_t, int16_t, int32_t, int64_t,
                                  float, double>;

bool matchesType(InputType type, const InputVariant &value)
{
    switch (type)
    {
    case InputType::BOOLEAN:
        return std::holds_alternative<bool>(value);
    case InputType::UINT8:
        return std::holds_alternative<uint8_t>(value);
    case InputType::UINT16:
        return std::holds_alternative<uint16_t>(value);
    case InputType::UINT32:
        return std::holds_alternative<uint32_t>(value);
    case InputType::UINT64:
        return std::holds_alternative<uint64_t>(value);
    case InputType::INT8:
        return std::holds_alternative<int8_t>(value);
    case InputType::INT16:
        return std::holds_alternative<int16_t>(value);
    case InputType::INT32:
        return std::holds_alternative<int32_t>(value);
    case InputType::INT64:
        return std::holds_alternative<int64_t>(value);
    case InputType::FLOAT:
        return std::holds_alternative<float>(value);
    case InputType::DOUBLE:
        return std::holds_alternative<double>(value);
    }
    return false;
}

InputVariant defaultValue(InputType type)
{
    switch (type)
    {
    case InputType::BOOLEAN:
        return false;
    case InputType::UINT8:
        return static_cast<uint8_t>(0);
    case InputType::UINT16:
        return static_cast<uint16_t>(0);
    case InputType::UINT32:
        return static_cast<uint32_t>(0);
    case InputType::UINT64:
        return static_cast<uint64_t>(0);
    case InputType::INT8:
        return static_cast<int8_t>(0);
    case InputType::INT16:
        return static_cast<int16_t>(0);
    case InputType::INT32:
        return static_cast<int32_t>(0);
    case InputType::INT64:
        return static_cast<int64_t>(0);
    case InputType::FLOAT:
        return 0.0f;
    case InputType::DOUBLE:
        return 0.0;
    }
    return false;
}
} // namespace

bool InputLayout::validate() const
{
    for (const auto &binding : keyboardBindings)
    {
        if (binding.keys.empty())
        {
            std::cerr << "InputBinding_Keyboard with id " << binding.id << " has no keys." << std::endl;
            return false;
        }

        if (!matchesType(binding.type, binding.value))
        {
            std::cerr << "InputBinding_Keyboard with id " << binding.id << " has an invalid value type." << std::endl;
            return false;
        }
    }
    return true;
}

PixL_Input_Handler::PixL_Input_Handler() = default;

PixL_Input_Handler::~PixL_Input_Handler() = default;

PixL_Input_Handler *PixL_Input_Handler::_instance = nullptr;

void PixL_Input_Handler::destroyInstance()
{
    delete _instance;
    _instance = nullptr;
}

bool PixL_Input_Handler::use_layout(const InputLayout &layout)
{
    if (!layout.validate())
    {
        std::cerr << "Invalid input layout provided." << std::endl;
        return false;
    }

    currentLayout = layout;
    layoutWarningIssued = false;

    inputValues.clear();
    for (const auto &binding : currentLayout.keyboardBindings)
    {
        inputValues[binding.id] = defaultValue(binding.type);
    }

    return true;
}

bool PixL_Input_Handler::update(const std::set<SDL_Keycode> &keys)
{
    if (currentLayout.keyboardBindings.empty())
    {
        if (!layoutWarningIssued)
        {
            std::cerr << "No layout loaded." << std::endl;
            layoutWarningIssued = true;
        }
        return false;
    }

    bool anyBindingActive = false;

    for (const auto &binding : currentLayout.keyboardBindings)
    {
        const bool isPressed = std::all_of(
            binding.keys.begin(),
            binding.keys.end(),
            [&](SDL_Keycode key)
            { return keys.count(key) != 0; });

        if (isPressed)
        {
            inputValues[binding.id] = binding.value;
            anyBindingActive = true;
        }
        else
        {
            inputValues[binding.id] = defaultValue(binding.type);
        }
    }

    return anyBindingActive;
}

std::map<uint32_t, std::variant<bool, uint8_t, uint16_t, uint32_t, uint64_t,
                                int8_t, int16_t, int32_t, int64_t,
                                float, double>>
PixL_Input_Handler::getInputValues()
{
    return inputValues;
}
