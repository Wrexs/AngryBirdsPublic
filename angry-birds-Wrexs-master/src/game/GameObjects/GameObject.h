#pragma once
#include "Components/SpriteComponent.h"
#include <Engine/Renderer.h>
#include <Engine/Sprite.h>
#include <string>

/**
 *  Objects used throughout the game.
 *  Provides a nice solid base class for objects in this game world.
 *  They currently support only sprite components, but this could easily
 *  be extended to include things like rigid bodies or input systems.
 *  @see SpriteComponent
 */
class GameObject
{
 public:
  /**
   *  Default constructor.
   */
  GameObject() = default;

  /**
   *  Destructor. Frees dynamic memory.
   */
  ~GameObject();

  void applyGravity(double);
  void setType(int);
  void setVisible(bool);
  void setMove(float, float);
  void setSpeed(float);
  void setPos(float, float);
  void setxSpacing(float);

  int getType();

  float getxSpacing();
  float getxMovement(double);
  float getyMovement(double);
  float getSpeed();
  ASGE::Point2D getMove();
  ASGE::Point2D getOrigin();
  ASGE::SpriteBounds getSpriteBounds();
  SpriteComponent* spriteComponent();
  bool addSpriteComponent(
    ASGE::Renderer* renderer, const std::string& texture_file_name);
  bool collisionCheckBounds(ASGE::SpriteBounds, float, float);
  bool collisionCheckCoords(double, double);
  bool getVisibile();

 private:
  void free();
  bool visible                      = false;
  float speed                       = 0;
  float x_spacing                   = 0;
  int type                          = 0;
  ASGE::Point2D object_origin       = { 0, 0 };
  ASGE::Point2D direction           = { 0, 0 };
  SpriteComponent* sprite_component = nullptr;
};
