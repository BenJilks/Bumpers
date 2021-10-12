#pragma once

namespace Engine::Input
{

	enum class MouseButton
	{
		Left = 0,
		Middle,
		Right,
	};

	bool is_key_down(char key);
	bool is_key_down(int special);
	bool is_mouse_button_down(MouseButton);
	bool is_mouse_button_click(MouseButton);
	int mouse_x();
	int mouse_y();

	void update_key_state(int key_code, bool state);
	void update_mouse_position(int x, int y);
	void update_mouse_button_state(MouseButton, bool state);
	void end_frame();

}
