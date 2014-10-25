#ifndef __CTRLHOOK_CONTROLLER_H_
#define __CTRLHOOK_CONTROLLER_H_

#include <cstdint>

namespace Hook
{
	enum class Buttons
	{
		BUTTON_Y = 0, BUTTON_B = 1, BUTTON_A = 2, BUTTON_X = 3, BUTTON_LB = 4, BUTTON_RB = 5,
		BUTTON_LT = 6, BUTTON_RT = 7, BUTTON_BACK = 8, BUTTON_START = 9, BUTTON_LS = 10,
		BUTTON_RS = 11
	};
	const static unsigned int NUM_BUTTONS = 12;

	struct ButtonFieldSecondary
	{
		uint8_t hat_up : 1;
		uint8_t hat_down : 1;
		uint8_t hat_left : 1;
		uint8_t hat_right : 1;

		uint8_t start : 1;
		uint8_t back : 1;
		uint8_t left_stick : 1;
		uint8_t right_stick : 1;
	};
	static_assert(sizeof(ButtonFieldSecondary) == 1, "ButtonFieldSecondary overflows 1 byte!");

	struct ButtonFieldPrimary
	{
		uint8_t left_shoulder : 1;
		uint8_t right_shoulder : 1;
		uint8_t guide : 1;
		// Empty spot
		uint8_t : 1;
		uint8_t button_a : 1;
		uint8_t button_b : 1;
		uint8_t button_x : 1;
		uint8_t button_y : 1;
	};
	static_assert(sizeof(ButtonFieldPrimary) == 1, "ButtonFieldPrimary overflows 1 byte!");

	struct ControllerState
	{
		uint32_t magic;
		uint32_t serial;
		uint8_t unk1;
		uint8_t magic2;

		// state[10] button bitfield
		ButtonFieldSecondary button_secondary;

		// state[11] button bitfield
		ButtonFieldPrimary button_primary;

		uint8_t left_trigger;
		uint8_t right_trigger;

		int16_t left_stick_x;
		int16_t left_stick_y;

		int16_t right_stick_x;
		int16_t right_stick_y;

		uint16_t unk2;
		uint32_t unk3;
	};
	static_assert(sizeof(ControllerState) == 28, "ControllerState larger than 28 bytes!");

	void initialiseState(ControllerState& state, unsigned int controllerNum);

#pragma region inlines

	inline void initialiseState(ControllerState& state, unsigned int controllerNum)
	{
		state.magic = 0x1C; // Assuming little-endian

		state.serial = controllerNum;

		state.unk1 = 0;

		state.magic2 = 0x14;

		state.unk2 = 0;
		state.unk3 = 0;
	}

#pragma endregion
}

#endif