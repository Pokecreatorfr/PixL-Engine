#include <PixL-Input-Handler.hpp>

bool InputLayout::validate() const
{
    for (const auto &binding : keyboardBindings)
    {
        if (binding.keys.empty())
        {
            std::cerr << "InputBinding_Keyboard with id " << binding.id << " has no keys." << std::endl;
            return false;
        }
        if (binding.type == InputType::BOOLEAN && !std::holds_alternative<bool>(binding.value))
        {
            std::cerr << "InputBinding_Keyboard with id " << binding.id << " has an invalid value type." << std::endl;
            return false;
        }
        if (binding.type == InputType::UINT8 && !std::holds_alternative<uint8_t>(binding.value))
        {
            std::cerr << "InputBinding_Keyboard with id " << binding.id << " has an invalid value type." << std::endl;
            return false;
        }
        if (binding.type == InputType::UINT16 && !std::holds_alternative<uint16_t>(binding.value))
        {
            std::cerr << "InputBinding_Keyboard with id " << binding.id << " has an invalid value type." << std::endl;
            return false;
        }
        if (binding.type == InputType::UINT32 && !std::holds_alternative<uint32_t>(binding.value))
        {
            std::cerr << "InputBinding_Keyboard with id " << binding.id << " has an invalid value type." << std::endl;
            return false;
        }
        if (binding.type == InputType::UINT64 && !std::holds_alternative<uint64_t>(binding.value))
        {
            std::cerr << "InputBinding_Keyboard with id " << binding.id << " has an invalid value type." << std::endl;
            return false;
        }
        if (binding.type == InputType::INT8 && !std::holds_alternative<int8_t>(binding.value))
        {
            std::cerr << "InputBinding_Keyboard with id " << binding.id << " has an invalid value type." << std::endl;
            return false;
        }
        if (binding.type == InputType::INT16 && !std::holds_alternative<int16_t>(binding.value))
        {
            std::cerr << "InputBinding_Keyboard with id " << binding.id << " has an invalid value type." << std::endl;
            return false;
        }
        if (binding.type == InputType::INT32 && !std::holds_alternative<int32_t>(binding.value))
        {
            std::cerr << "InputBinding_Keyboard with id " << binding.id << " has an invalid value type." << std::endl;
            return false;
        }
        if (binding.type == InputType::INT64 && !std::holds_alternative<int64_t>(binding.value))
        {
            std::cerr << "InputBinding_Keyboard with id " << binding.id << " has an invalid value type." << std::endl;
            return false;
        }
        if (binding.type == InputType::FLOAT && !std::holds_alternative<float>(binding.value))
        {
            std::cerr << "InputBinding_Keyboard with id " << binding.id << " has an invalid value type." << std::endl;
            return false;
        }
        if (binding.type == InputType::DOUBLE && !std::holds_alternative<double>(binding.value))
        {
            std::cerr << "InputBinding_Keyboard with id " << binding.id << " has an invalid value type." << std::endl;
            return false;
        }
    }
    return true;
}

PixL_Input_Handler::PixL_Input_Handler()
{
}

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
    inputValues.clear();
    layoutWarningIssued = false;
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
        return true;
    }
    for (const auto &binding : currentLayout.keyboardBindings)
    {
        bool isAllRequiredKeysPressed = true;
        for (const auto &Key : binding.keys)
        {
            if (keys.find(Key) == keys.end())
            {
                isAllRequiredKeysPressed = false;
                break;
            }
        }
        if (!isAllRequiredKeysPressed)
        {
            inputValues.erase(binding.id);
            continue;
        }

        switch (binding.type)
        {
        case InputType::BOOLEAN:
            inputValues[binding.id] = std::get<bool>(binding.value);
            break;
        case InputType::UINT8:
            inputValues[binding.id] = std::get<uint8_t>(binding.value);
            break;
        case InputType::UINT16:
            inputValues[binding.id] = std::get<uint16_t>(binding.value);
            break;
        case InputType::UINT32:
            inputValues[binding.id] = std::get<uint32_t>(binding.value);
            break;
        case InputType::UINT64:
            inputValues[binding.id] = std::get<uint64_t>(binding.value);
            break;
        case InputType::INT8:
            inputValues[binding.id] = std::get<int8_t>(binding.value);
            break;
        case InputType::INT16:
            inputValues[binding.id] = std::get<int16_t>(binding.value);
            break;
        case InputType::INT32:
            inputValues[binding.id] = std::get<int32_t>(binding.value);
            break;
        case InputType::INT64:
            inputValues[binding.id] = std::get<int64_t>(binding.value);
            break;
        case InputType::FLOAT:
            inputValues[binding.id] = std::get<float>(binding.value);
            break;
        case InputType::DOUBLE:
            inputValues[binding.id] = std::get<double>(binding.value);
            break;
        default:
            std::cerr << "Unknown type for binding id " << binding.id << std::endl;
            break;
        }
    }

    return true;
}

std::map<uint32_t, std::variant<bool, uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t, float, double>> PixL_Input_Handler::getInputValues()
{
    return inputValues;
}
