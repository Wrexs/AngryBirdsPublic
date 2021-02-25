//
// Created by zacpa on 08/04/2020.
//

#include "EnemyObjects.h"

void EnemyObjects::moveEnemies(double dt_sec)
{
  auto enemy_xmove = obj.getMove().x;
  auto enemy_ymove = obj.getMove().y;
  auto enemy_xpos  = obj.spriteComponent()->getSprite()->xPos();
  auto enemy_ypos  = obj.spriteComponent()->getSprite()->yPos();
  auto new_xpos    = enemy_xpos + enemy_xmove * dt_sec * this->obj.getSpeed();
  auto new_ypos    = enemy_ypos + enemy_ymove * dt_sec * this->obj.getSpeed();

  obj.setPos(static_cast<float>(new_xpos), static_cast<float>(new_ypos));
}

void EnemyObjects::reset()
{
  obj.setMove(0, 0);
  obj.setVisible(true);
  obj.setSpeed(1);
  obj.spriteComponent()->getSprite()->width(35);
  obj.spriteComponent()->getSprite()->height(35);
}
