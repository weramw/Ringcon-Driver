#pragma once
#include "JoyCon.h"

#include <string>

class JoyConLeft :
    public JoyCon
{
public:

    // true = pressed
    struct ButtonsState {
        bool DOWN;
        bool UP;
        bool RIGHT;
        bool LEFT;
        bool SR;
        bool SL;
        bool L;
        bool ZL;
        bool MINUS;
        bool STICK;
        bool CAPTURE;
        ButtonsState();
        std::string toString() const;
        
    };

    JoyConLeft(hid_device_info* dev_info);

    void parseButtonsState(const std::vector<uint8_t>& data);
    int getStickDataOffset() const;
    std::string getButtonsStateAsString() const;

    ButtonsState getButtonsState() const;

    virtual JoyData getData() const override;

protected:
    ButtonsState _buttons_state;
};

