#include "Controllers/Dualshock3Controller.h"

#define LED_PERMANENT 0xff, 0x27, 0x00, 0x00, 0x32

static_assert(sizeof(Dualshock3ButtonData) == 49);

Dualshock3Controller::Dualshock3Controller(std::unique_ptr<IUSBDevice> &&device, const ControllerConfig &config, std::unique_ptr<ILogger> &&logger)
    : BaseController(std::move(device), config, std::move(logger))
{
}

Dualshock3Controller::~Dualshock3Controller()
{
}

ControllerResult Dualshock3Controller::Initialize()
{
    ControllerResult result = BaseController::Initialize();
    if (result != CONTROLLER_STATUS_SUCCESS)
        return result;

    SetLED(DS3LED_1);

    return CONTROLLER_STATUS_SUCCESS;
}

ControllerResult Dualshock3Controller::OpenInterfaces()
{
    ControllerResult result = BaseController::OpenInterfaces();
    if (result != CONTROLLER_STATUS_SUCCESS)
        return result;

    constexpr uint8_t initBytes[] = {0x42, 0x0C, 0x00, 0x00};
    result = SendCommand(Ds3FeatureStartDevice, initBytes, sizeof(initBytes));
    if (result != CONTROLLER_STATUS_SUCCESS)
        return result;

    return CONTROLLER_STATUS_SUCCESS;
}

ControllerResult Dualshock3Controller::ParseData(uint8_t *buffer, size_t size, RawInputData *rawData, uint16_t *input_idx)
{
    (void)input_idx;
    Dualshock3ButtonData *buttonData = reinterpret_cast<Dualshock3ButtonData *>(buffer);

    if (size < sizeof(Dualshock3ButtonData))
        return CONTROLLER_STATUS_UNEXPECTED_DATA;

    if (buttonData->type == Ds3InputPacket_Button)
    {
        rawData->buttons[1] = buttonData->button1;
        rawData->buttons[2] = buttonData->button2;
        rawData->buttons[3] = buttonData->button3;
        rawData->buttons[4] = buttonData->button4;
        rawData->buttons[5] = buttonData->button5;
        rawData->buttons[6] = buttonData->button6;
        rawData->buttons[7] = buttonData->button7;
        rawData->buttons[8] = buttonData->button8;
        rawData->buttons[9] = buttonData->button9;
        rawData->buttons[10] = buttonData->button10;
        rawData->buttons[11] = buttonData->button11;
        rawData->buttons[12] = buttonData->button12;
        rawData->buttons[13] = buttonData->button13;

        rawData->analog[ControllerAnalogType_Rx] = BaseController::Normalize(buttonData->Rx, 0, 255);
        rawData->analog[ControllerAnalogType_Ry] = BaseController::Normalize(buttonData->Ry, 0, 255);
        rawData->analog[ControllerAnalogType_X] = BaseController::Normalize(buttonData->X, 0, 255);
        rawData->analog[ControllerAnalogType_Y] = BaseController::Normalize(buttonData->Y, 0, 255);
        rawData->analog[ControllerAnalogType_Z] = BaseController::Normalize(buttonData->Z, 0, 255);
        rawData->analog[ControllerAnalogType_Rz] = BaseController::Normalize(buttonData->Rz, 0, 255);

        rawData->buttons[DPAD_UP_BUTTON_ID] = buttonData->dpad_up;
        rawData->buttons[DPAD_RIGHT_BUTTON_ID] = buttonData->dpad_right;
        rawData->buttons[DPAD_DOWN_BUTTON_ID] = buttonData->dpad_down;
        rawData->buttons[DPAD_LEFT_BUTTON_ID] = buttonData->dpad_left;

        return CONTROLLER_STATUS_SUCCESS;
    }

    return CONTROLLER_STATUS_UNEXPECTED_DATA;
}

ControllerResult Dualshock3Controller::SendCommand(Dualshock3FeatureValue feature, const void *buffer, uint16_t size)
{
    return m_interfaces[0]->ControlTransferOutput(0x21, 0x09, static_cast<uint16_t>(feature), 0, buffer, size);
}

ControllerResult Dualshock3Controller::SetLED(Dualshock3LEDValue value)
{
    const uint8_t ledPacket[]{
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        static_cast<uint8_t>(value << 1),
        LED_PERMANENT,
        LED_PERMANENT,
        LED_PERMANENT,
        LED_PERMANENT};
    return SendCommand(Ds3FeatureUnknown1, ledPacket, sizeof(ledPacket));
}