#include "input.hpp"
#include <unordered_set>
#include <array>
#include <cassert>
using namespace Engine;

static std::unordered_set<int> s_keys_down;
static std::array<bool, 3> s_mouse_button_down;
static std::array<bool, 3> s_mouse_button_click;
static int s_mouse_x { 0 };
static int s_mouse_y { 0 };

bool Input::is_key_down(char key)
{
	return s_keys_down.contains(key);
}

bool Input::is_key_down(int special)
{
	return s_keys_down.contains(special << 8);
}

bool Input::is_mouse_button_down(MouseButton button)
{
	return s_mouse_button_down[(size_t)button];
}

bool Input::is_mouse_button_click(MouseButton button)
{
	return s_mouse_button_click[(size_t)button];
}

int Input::mouse_x()
{
	return s_mouse_x;
}

int Input::mouse_y()
{
	return s_mouse_y;
}

void Input::update_key_state(int key_code, bool state)
{
	if (state)
		s_keys_down.insert(key_code);
	else
		s_keys_down.erase(key_code);
}

void Input::update_mouse_position(int x, int y)
{
	s_mouse_x = x;
	s_mouse_y = y;
}

void Input::update_mouse_button_state(MouseButton button, bool state)
{
	if (state)
		s_mouse_button_click[(size_t)button] = true;

	s_mouse_button_down[(size_t)button] = state;
}

void Input::end_frame()
{
	s_mouse_button_click.fill(false);
}
