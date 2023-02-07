#pragma once
#include "JoyCon.h"
class JoyConRight :
    public JoyCon
{
public:
    // true = pressed
    struct ButtonsState {
        bool Y;
        bool X;
        bool B;
        bool A;
        bool SR;
        bool SL;
        bool R;
        bool ZR;
        bool PLUS;
        bool STICK;
        bool HOME;
        ButtonsState();
        std::string toString() const;
    };

    JoyConRight(hid_device_info* dev_info);

    void parseButtonsState(const std::vector<uint8_t>& data);
    int getStickDataOffset() const;
    std::string getButtonsStateAsString() const;

    ButtonsState getButtonsState() const;

protected:
    ButtonsState _buttons_state;
};

