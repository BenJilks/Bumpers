#include "spin.hpp"
#include "gameobject/gameobject.hpp"
#include "gameobject/transform.hpp"
#include <glm/glm.hpp>
#include <iostream>
using namespace Game;
using namespace Object;
using namespace glm;

void Spin::init(Object::GameObject &gameobject)
{
    m_transform = gameobject.first<Transform>();
}

void Spin::update(Object::GameObject &gameobject, float delta) 
{
    assert(m_transform);

    m_transform->set_rotation(vec3(m_counter / 70.0f));
    m_counter += 1;
}
