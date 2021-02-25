#include "GameObject.h"
#include <Engine/Renderer.h>
#include <cmath>

GameObject::~GameObject()
{
  free();
}

bool GameObject::addSpriteComponent(
  ASGE::Renderer* renderer, const std::string& texture_file_name)
{
  free();
  sprite_component = new SpriteComponent();
  if (sprite_component->loadSprite(renderer, texture_file_name))
  {
    return true;
  }

  free();
  return false;
}

void GameObject::free()
{
  delete sprite_component;
  sprite_component = nullptr;
}

SpriteComponent* GameObject::spriteComponent()
{
  return sprite_component;
}

bool GameObject::getVisibile()
{
  return visible;
}

void GameObject::setVisible(bool new_visible)
{
  visible = new_visible;
}

void GameObject::setMove(float xMove, float yMove)
{
  direction.x = xMove;
  direction.y = yMove;
}

ASGE::Point2D GameObject::getMove()
{
  return direction;
}

float GameObject::getSpeed()
{
  return speed;
}

void GameObject::setSpeed(float speed_in)
{
  speed = speed_in;
}

bool GameObject::collisionCheckBounds(
  ASGE::SpriteBounds Other, float objectXMove, float objectYMove)
{
  ASGE::SpriteBounds spriteBoundsA =
    this->spriteComponent()->getSprite()->getLocalBounds();

  ASGE::SpriteBounds spriteBoundsB = Other;

  // spriteBounds collision check
  if (
    spriteBoundsA.v1.x + objectXMove < spriteBoundsB.v2.x &&
    spriteBoundsA.v2.x + objectXMove > spriteBoundsB.v1.x &&
    spriteBoundsA.v1.y + objectYMove < spriteBoundsB.v3.y &&
    spriteBoundsA.v3.y + objectYMove > spriteBoundsB.v1.y)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool GameObject::collisionCheckCoords(double x_pos, double y_pos)
{
  ASGE::SpriteBounds spriteBoundsA =
    this->spriteComponent()->getSprite()->getLocalBounds();
  if (
    spriteBoundsA.v1.x < x_pos && spriteBoundsA.v2.x > x_pos &&
    spriteBoundsA.v1.y < y_pos && spriteBoundsA.v3.y > y_pos)
  {
    return true;
  }
  else
  {
    return false;
  }
}

void GameObject::setPos(float x, float y)
{
  spriteComponent()->getSprite()->xPos(x);
  spriteComponent()->getSprite()->yPos(y);
}

float GameObject::getxMovement(double dt_sec)
{
  float x_movement = 0;

  x_movement = (static_cast<float>(getMove().x * dt_sec * getSpeed()));

  return x_movement;
}

float GameObject::getyMovement(double dt_sec)
{
  float y_movement = 0;

  y_movement = (static_cast<float>(getMove().y * dt_sec * getSpeed()));

  return y_movement;
}

ASGE::SpriteBounds GameObject::getSpriteBounds()
{
  return this->spriteComponent()->getSprite()->getLocalBounds();
}

float GameObject::getxSpacing()
{
  return x_spacing;
}

void GameObject::setxSpacing(float new_x_spacing)
{
  x_spacing = new_x_spacing;
}

void GameObject::setType(int new_type)
{
  type = new_type;
}

int GameObject::getType()
{
  return type;
}

void GameObject::applyGravity(double dt_sec)
{
  this->setMove(direction.x, static_cast<float>(direction.y + 500 * dt_sec));
}

ASGE::Point2D GameObject::getOrigin()
{
  auto xpos   = spriteComponent()->getSprite()->xPos();
  auto ypos   = spriteComponent()->getSprite()->yPos();
  auto width  = spriteComponent()->getSprite()->width();
  auto height = spriteComponent()->getSprite()->height();

  object_origin = { xpos + width / 2, ypos + height / 2 };

  return object_origin;
}
