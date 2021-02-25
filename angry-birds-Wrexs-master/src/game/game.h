#pragma once
#include <Engine/Camera2D.hpp>
#include <Engine/OGLGame.h>
#include <GameObjects/BirdObjects.h>
#include <GameObjects/EnemyObjects.h>
#include <GameObjects/GameObject.h>
#include <string>

/**
 *  An OpenGL Game based on ASGE.
 */
class AngryBirds : public ASGE::OGLGame
{
 public:
  explicit AngryBirds(ASGE::GameSettings settings);
  ~AngryBirds() final;
  bool init();
  bool initBirds();
  bool initEnemies();
  bool initBlocks();
  bool initScene();

 private:
  void keyHandler(ASGE::SharedEventData data);
  void clickHandler(ASGE::SharedEventData data);
  void update(const ASGE::GameTime&) override;
  void render() override;
  void moveCurrentBird();
  void adjustBirds();
  void fireBird();
  void birdsCollision(double);
  void enemyCollision(double);
  void extraCollision(double);
  void extraMovement(double);
  void endGame();
  void storeMousePos(ASGE::SharedEventData data);
  void logStartup() const;

  int key_callback_id   = -1; /**< Key Input Callback ID. */
  int mouse_callback_id = -1; /**< Mouse Input Callback ID. */

  bool bird_held         = false;
  bool show_instructions = false;
  bool in_menu           = true;
  bool game_over         = false;

  ASGE::Sprite* menu_background  = nullptr;
  ASGE::Sprite* level_background = nullptr;
  ASGE::Sprite* begin            = nullptr;
  ASGE::Sprite* instructions     = nullptr;
  ASGE::Camera2D camera;

  ASGE::Point2D mouse_pos;
  ASGE::Point2D slingshot_origin = { 325, 500 };

  float max_distance = 60;

  static const int MAX_BLOCKS  = 10;
  static const int MAX_BIRDS   = 6;
  static const int MAX_ENEMIES = 5;
  int birds_left               = MAX_BIRDS;
  int blocks_left              = MAX_BLOCKS;
  int enemies_left             = MAX_ENEMIES;
  int current_bird             = 0;

  GameObject blocks[MAX_BLOCKS];
  EnemyObjects enemies[MAX_ENEMIES];
  BirdObjects birds[MAX_BIRDS];
  GameObject* road_roller = new GameObject;
  GameObject* slingshot   = new GameObject;
  GameObject* ground      = new GameObject;
  ASGE::Text win_lose{};
  std::string win_lose_string;
};