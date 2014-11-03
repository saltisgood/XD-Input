#ifndef __CTRLHOOK_CONTROLLER_H_
#define __CTRLHOOK_CONTROLLER_H_

#include <cstdint>
#include <string>

namespace Hook
{
	// An enum of controller buttons using Xbox 360 controller nomenclature
	enum class Buttons
	{
		BUTTON_Y = 0, BUTTON_B = 1, BUTTON_A = 2, BUTTON_X = 3, BUTTON_LB = 4, BUTTON_RB = 5,
		BUTTON_LT = 6, BUTTON_RT = 7, BUTTON_BACK = 8, BUTTON_START = 9, BUTTON_LS = 10,
		BUTTON_RS = 11, INVALID
	};
	// The total number of valid buttons in the Hook::Buttons enum
	const static unsigned int NUM_BUTTONS = 12;

	// The bitfield of the secondary buttons, used in Hook::ControllerState
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

	// The bitfield of the primary buttons, used in Hook::ControllerState
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

	// The state of the controller that's used by the Scp driver. Call initialiseState(ControllerState&, unsigned int);
	// before use to make sure it's in a valid state.
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
	static_assert(sizeof(ControllerState) == 28, "ControllerState not exactly 28 bytes!");

	// Get the ControllerState into a valid state for use. Valid values for controllerNum are 1-4.
	void initialiseState(ControllerState& state, unsigned int controllerNum);

	// Parse a phrase to get its Buttons enum form.
	Buttons parseButtonString(const std::string& str);
	// Turn a value of the Buttons enum into its string representation.
	const std::string& getButtonString(Buttons butt);

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