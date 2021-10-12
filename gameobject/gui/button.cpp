#include "button.hpp"
#include "control.hpp"
#include "engine/input.hpp"
#include "engine/display.hpp"
#include "gameobject/gameobject.hpp"
#include "gameobject/transform.hpp"
#include "gameobject/mesh_render.hpp"
#include <cassert>
#include <iostream>
using namespace Object::Gui;
using namespace Engine;

void Button::init(GameObject &gameobject)
{
    m_transform = gameobject.first<Transform>();
    m_control = gameobject.first<Control>();
    m_mesh_render = gameobject.first<MeshRender>();
}

void Button::update(GameObject&, float delta)
{
    assert (m_transform);
    assert (m_control);
    assert (m_mesh_render);

    const auto &anchor = m_control->anchor();
    int x = Display::width() * (anchor.x * 0.5f + 0.5f) + m_transform->position().x;
    int y = Display::height() * (anchor.y * 0.5f + 0.5f) - m_transform->position().y;
    int width = m_transform->scale().x;
    int height = m_transform->scale().y;

    m_hovered = Input::mouse_x() > x - width && Input::mouse_x() < x + width 
        && Input::mouse_y() > y - height && Input::mouse_y() < y + height;
    m_mesh_render->set_diffuse(m_hovered ? m_hover : m_normal);

    if (on_click && m_hovered && Input::is_mouse_button_click(Input::MouseButton::Left))
        on_click();
}
