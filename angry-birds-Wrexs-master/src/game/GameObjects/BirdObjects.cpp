//
// Created by zacpa on 08/04/2020.
//

#include "BirdObjects.h"
#include <Engine/Renderer.h>

void BirdObjects::setFired(bool new_fired)
{
  fired = new_fired;
}

bool BirdObjects::getFired()
{
  return fired;
}

BirdObjects::~BirdObjects() {}

void BirdObjects::moveBirds(double dt_sec)
{
  auto current_xpos = obj.spriteComponent()->getSprite()->xPos();
  auto current_ypos = obj.spriteComponent()->getSprite()->yPos();
  auto window_width = static_cast<float>(ASGE::SETTINGS.window_width);

  if (fired)
  {
    if (!ignore_gravity)
    {
      obj.applyGravity(dt_sec);
    }

    auto x_movement = obj.getMove().x * dt_sec * obj.getSpeed();
    auto y_movement = obj.getMove().y * dt_sec * obj.getSpeed();
    auto new_xpos   = static_cast<float>(current_xpos + x_movement);
    auto new_ypos   = static_cast<float>(current_ypos + y_movement);

    obj.setPos(new_xpos, new_ypos);

    if (current_xpos < 0 || current_xpos > window_width)
    {
      obj.setMove(0, 0);
    }
  }
}

void BirdObjects::activateBirdPower()
{
  if (!activated_power)
  {
    auto type = obj.getType();

    if (type == 1)
    {
      max_collides = 0;
    }

    else if (type == 2)
    {
      auto bird_width  = obj.spriteComponent()->getSprite()->width();
      auto bird_height = obj.spriteComponent()->getSprite()->height();

      obj.spriteComponent()->getSprite()->width(bird_width * 2);
      obj.spriteComponent()->getSprite()->height(bird_height * 2);
      max_collides = 2;
    }
    else if (type == 3)
    {
      obj.setMove(0, obj.getMove().y);
    }
    else if (type == 4)
    {
      obj.setMove(obj.getMove().x * 2, obj.getMove().y);
      max_collides = 2;
    }
    else if (type == 5)
    {
      obj.setMove(0, -1200);
    }
  }

  activated_power = true;
}

void BirdObjects::increaseCollided()
{
  collided++;
}

bool BirdObjects::getActivatedPower()
{
  return activated_power;
}

void BirdObjects::setIgnoreBlock(int new_ignore_block)
{
  ignore_block = new_ignore_block;
}

void BirdObjects::setMaxCollides(int new_max_collides)
{
  max_collides = new_max_collides;
}

void BirdObjects::setIgnoreGravity(bool new_ignore_gravity)
{
  ignore_gravity = new_ignore_gravity;
}

void BirdObjects::reset()
{
  obj.setMove(0, 0);
  fired    = false;
  collided = 0;
  obj.setSpeed(1);
  obj.setVisible(true);
  activated_power = false;
  ignore_gravity  = false;
  ignore_block    = 100;
}

bool BirdObjects::getCollidePossible()
{
  if (collided < max_collides)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool BirdObjects::checkIgnoreBlock(int check_number)
{
  if (ignore_block == check_number)
  {
    return true;
  }
  else
  {
    return false;
  }
}
