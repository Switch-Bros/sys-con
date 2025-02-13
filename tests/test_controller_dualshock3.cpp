#include <gtest/gtest.h>
#include "Controllers/Dualshock3Controller.h"
#include "mocks/Device.h"
#include "mocks/Logger.h"

TEST(Controller, test_dualshock3_dpad_right)
{
    ControllerConfig config;
    RawInputData rawData;
    uint16_t input_idx = 0;
    Dualshock3Controller controller(std::make_unique<MockDevice>(), config, std::make_unique<MockLogger>());

    uint8_t buffer[64] = {0x01, 0x00, 0x20, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xEF, 0x14,
                          0x00, 0x00, 0x00, 0x00, 0x23, 0x18, 0x77, 0x01, 0x1D, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02,
                          0x00};
    EXPECT_EQ(controller.ParseData(buffer, sizeof(buffer), &rawData, &input_idx), CONTROLLER_STATUS_SUCCESS);

    EXPECT_TRUE(rawData.buttons[DPAD_RIGHT_BUTTON_ID]);
}
