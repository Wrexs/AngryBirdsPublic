//
// Created by zacpa on 08/04/2020.
//

#ifndef ANGRYBIRDS_BIRDOBJECTS_H

#include "GameObject.h"
#include <Engine/Renderer.h>
#define ANGRYBIRDS_BIRDOBJECTS_H

class BirdObjects
{
 public:
  /**
   *  Default constructor.
   */
  BirdObjects() = default;

  /**
   *  Destructor. Frees dynamic memory.
   */
  ~BirdObjects();
  GameObject obj;

  void setFired(bool);
  void moveBirds(double);
  void activateBirdPower();
  void increaseCollided();
  void setIgnoreBlock(int);
  void setIgnoreGravity(bool);
  void setMaxCollides(int);
  void reset();
  bool getCollidePossible();
  bool getFired();
  bool checkIgnoreBlock(int);
  bool getActivatedPower();

 private:
  bool fired           = false;
  bool activated_power = false;
  bool ignore_gravity  = false;
  int collided         = 0;
  int max_collides     = 1;
  int ignore_block     = 100;
};

#endif // ANGRYBIRDS_BIRDOBJECTS_H
