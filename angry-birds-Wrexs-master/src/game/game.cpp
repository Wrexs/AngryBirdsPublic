#include <string>

#include <Engine/InputEvents.h>
#include <Engine/Keys.h>
#include <Engine/Logger.hpp>
#include <Engine/Point2D.h>
#include <Engine/Sprite.h>

#include "game.h"
/**
 *   @brief   Default Constructor.
 */
AngryBirds::AngryBirds(ASGE::GameSettings settings) : OGLGame(settings)
{
  renderer->setClearColour(ASGE::COLOURS::SLATEGRAY);
  inputs->use_threads = false;
  // toggleFPS();

  // every game needs a good background
  menu_background = renderer->createRawSprite();
  menu_background->loadTexture("/data/images/menu.jpg");
  menu_background->width(static_cast<float>(ASGE::SETTINGS.window_width));
  menu_background->height(static_cast<float>(ASGE::SETTINGS.window_height));

  // create a camera pointing at the mid-point of the screen
  camera = ASGE::Camera2D{ static_cast<float>(ASGE::SETTINGS.window_width),
                           static_cast<float>(ASGE::SETTINGS.window_height) };

  camera.lookAt(
    ASGE::Point2D{ static_cast<float>(-ASGE::SETTINGS.window_width) / 2.f,
                   static_cast<float>(-ASGE::SETTINGS.window_height) / 2.f });

  logStartup();
}

void AngryBirds::logStartup() const
{
  // these are just examples of the logging system.. you dont need to keep em
  std::ostringstream s{ "initialised complete" };
  Logging::ERRORS(s.str());
  Logging::WARN(s.str());
  Logging::INFO(s.str());
  Logging::DEBUG(s.str());
  Logging::TRACE(s.str());
  Logging::log(
    Logging::timestamp() + " \x1b[35;1m[CUSTOM]\x1b[0m " + s.str() + '\n');
}

/**
 *   @brief   Destructor.
 *   @details Remove any non-managed memory and callbacks.
 */
AngryBirds::~AngryBirds()
{
  delete menu_background;
  delete level_background;
  delete begin;
  delete slingshot;
  delete ground;
  delete road_roller;
  this->inputs->unregisterCallback(static_cast<unsigned int>(key_callback_id));

  this->inputs->unregisterCallback(
    static_cast<unsigned int>(mouse_callback_id));
}

/**
 *   @brief   Initialises the game.
 *   @details The game window is created and all assets required to
 *            run the game are loaded. The keyHandler and clickHandler
 *            callback should also be set in the initialise function.
 *   @return  True if the game initialised correctly.
 */
bool AngryBirds::init()
{
  key_callback_id =
    inputs->addCallbackFnc(ASGE::E_KEY, &AngryBirds::keyHandler, this);

  mouse_callback_id = inputs->addCallbackFnc(
    ASGE::E_MOUSE_CLICK, &AngryBirds::clickHandler, this);

  begin = renderer->createRawSprite();
  begin->loadTexture("data/images/Begin.png");
  begin->xPos(
    static_cast<float>(ASGE::SETTINGS.window_width) / 2 - begin->width() / 2);
  begin->yPos(600);

  instructions = renderer->createRawSprite();
  instructions->loadTexture("data/images/Instructions.png");
  instructions->xPos(
    static_cast<float>(ASGE::SETTINGS.window_width) / 2 - begin->width() / 2);
  instructions->yPos(640);

  initScene();
  initBirds();
  initEnemies();
  initBlocks();
  current_bird = 2;

  mouse_callback_id = inputs->addCallbackFnc(
    ASGE::E_MOUSE_MOVE, &AngryBirds::storeMousePos, this);

  return true;
}

/**
 *   @brief   Processes any key inputs
 *   @details This function is added as a callback to handle the game's
 *            keyboard input. For this game, calls to this function
 *            are thread safe, so you may alter the game's state as
 *            you see fit.
 *   @param   data The event data relating to key input.
 *   @see     KeyEvent
 *   @return  void
 */
void AngryBirds::keyHandler(ASGE::SharedEventData data)
{
  auto key = static_cast<const ASGE::KeyEvent*>(data.get());

  if (key->key == ASGE::KEYS::KEY_ESCAPE)
  {
    signalExit();
  }
  else if (key->key == ASGE::KEYS::KEY_ENTER)
  {
    if (in_menu)
    {
      in_menu      = false;
      current_bird = 0;
    }
  }
  else if (key->key == ASGE::KEYS::KEY_I)
  {
    if (in_menu)
    {
      menu_background->loadTexture("data/images/Tutorial.png");
      menu_background->setGlobalZOrder(-1);
      show_instructions = true;
    }
  }
  else if (key->key == ASGE::KEYS::KEY_R)
  {
    if (!in_menu)
    {
      current_bird = 0;
      initBirds();
      initEnemies();
      initBlocks();
      initScene();
      game_over    = false;
      birds_left   = MAX_BIRDS;
      blocks_left  = MAX_BLOCKS;
      enemies_left = MAX_ENEMIES;
    }
  }
}

/**
 *   @brief   Processes any click inputs
 *   @details This function is added as a callback to handle the game's
 *            mouse button input. For this game, calls to this function
 *            are thread safe, so you may alter the game's state as you
 *            see fit.
 *   @param   data The event data relating to key input.
 *   @see     ClickEvent
 *   @return  void
 */
void AngryBirds::clickHandler(ASGE::SharedEventData data)
{
  auto click = static_cast<const ASGE::ClickEvent*>(data.get());

  double x_pos = click->xpos;
  double y_pos = click->ypos;
  auto bird_width =
    birds[current_bird].obj.spriteComponent()->getSprite()->width();
  auto bird_height =
    birds[current_bird].obj.spriteComponent()->getSprite()->height();

  Logging::TRACE("x_pos: " + std::to_string(x_pos));
  Logging::TRACE("y_pos: " + std::to_string(y_pos));

  if (birds[current_bird].getFired())
  {
    birds[current_bird].activateBirdPower();
  }

  if (
    birds[current_bird].obj.collisionCheckCoords(x_pos, y_pos) &&
    !birds[current_bird].getFired())
  {
    bird_held = true;
  }
  if (!click->action)
  {
    if (bird_held == true)
    {
      if (mouse_pos.distance(slingshot_origin) <= bird_width * 1.1)
      {
        birds[current_bird].obj.setPos(
          slingshot_origin.x - bird_width / 2,
          slingshot_origin.y - bird_height / 2);
      }
      else
      {
        fireBird();
      }

      bird_held = false;
    }
  }
}

void AngryBirds::storeMousePos(ASGE::SharedEventData data)
{
  auto mouse = static_cast<const ASGE::MoveEvent*>(data.get());
  mouse_pos  = ASGE::Point2D(
    static_cast<float>(mouse->xpos), static_cast<float>(mouse->ypos));
}

/**
 *   @brief   Updates the scene
 *   @details Prepares the renderer subsystem before drawing the
 *            current frame. Once the current frame is has finished
 *            the buffers are swapped accordingly and the image shown.
 *   @return  void
 */
void AngryBirds::update(const ASGE::GameTime& game_time)
{
  double dt_sec = game_time.delta.count() / 1000.0;

  if (!in_menu && !game_over)
  {
    camera.update(game_time);
    enemyCollision(dt_sec);
    birdsCollision(dt_sec);

    for (int i = 0; i < MAX_ENEMIES; i++)
    {
      enemies[i].moveEnemies(dt_sec);
    }
    for (int i = 0; i < MAX_BIRDS; i++)
    {
      birds[i].moveBirds(dt_sec);
    }
    adjustBirds();
    extraCollision(dt_sec);
    extraMovement(dt_sec);
    moveCurrentBird();
    if (enemies_left == 0 && birds[current_bird].getFired() == false)
    {
      game_over = true;
    }
  }
  if (game_over)
  {
    endGame();
  }
}

/**
 *   @brief   Renders the scene
 *   @details Renders all the game objects to the current frame.
 *            Once the current frame is has finished the buffers are
 *            swapped accordingly and the image shown.
 *   @return  void
 */
void AngryBirds::render()
{
  renderer->setFont(0);

  if (in_menu)
  {
    renderer->renderSprite(*menu_background);
    renderer->renderSprite(*begin);
    if (!show_instructions)
    {
      renderer->renderSprite(*instructions);
    }
  }
  else
  {
    renderer->renderSprite(*road_roller->spriteComponent()->getSprite());
    renderer->renderSprite(*slingshot->spriteComponent()->getSprite());
    renderer->renderSprite(*ground->spriteComponent()->getSprite());
    renderer->renderSprite(*level_background);

    for (int i = 0; i < MAX_BIRDS; i++)
    {
      if (birds[i].obj.getVisibile())
      {
        renderer->renderSprite(*birds[i].obj.spriteComponent()->getSprite());
      }
    }
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
      if (enemies[i].obj.getVisibile())
      {
        renderer->renderSprite(*enemies[i].obj.spriteComponent()->getSprite());
      }
    }

    for (int i = 0; i < MAX_BLOCKS; i++)
    {
      if (blocks[i].getVisibile())
      {
        renderer->renderSprite(*blocks[i].spriteComponent()->getSprite());
      }
    }

    renderer->setProjectionMatrix(camera.getView());

    if (game_over)
    {
      int bird_score  = birds_left * 500;
      int enemy_score = (MAX_ENEMIES - enemies_left) * 1000;
      int block_score = (MAX_BLOCKS - blocks_left) * 100;
      int score = (bird_score + enemy_score + (MAX_BLOCKS - blocks_left) * 100);

      ASGE::Text score_birds = { renderer->getDefaultFont(),
                                 "Birds left:        " +
                                   std::to_string(birds_left) +
                                   " x 500   =  " + std::to_string(bird_score),
                                 500,
                                 250 };

      ASGE::Text score_enemies = {
        renderer->getDefaultFont(),
        "Enemies destroyed: " + std::to_string(enemy_score / 1000) +
          " x 1000   =  " + std::to_string(enemy_score),
        500,
        300
      };

      ASGE::Text score_blocks = {
        renderer->getDefaultFont(),
        "Blocks destroyed: " + std::to_string(block_score / 100) +
          " x 100   =  " + std::to_string(block_score),
        500,
        350
      };

      ASGE::Text score_total = {
        renderer->getDefaultFont(), "Total: " + std::to_string(score), 650, 400
      };

      ASGE::Text score_title = {
        renderer->getDefaultFont(), "Score:", 600, 200
      };

      score_birds.setZOrder(100);
      score_enemies.setZOrder(100);
      score_blocks.setZOrder(100);
      score_total.setZOrder(100);
      score_title.setZOrder(100);

      win_lose = { renderer->getDefaultFont(),
                   "Game Over. You " + win_lose_string,
                   550,
                   150 };
      win_lose.setColour({ 0, 0, 0 });
      score_birds.setColour({ 0, 0, 0 });
      score_enemies.setColour({ 0, 0, 0 });
      score_blocks.setColour({ 0, 0, 0 });
      score_total.setColour({ 0, 0, 0 });
      score_title.setColour({ 0, 0, 0 });

      renderer->renderText(score_birds);
      renderer->renderText(score_enemies);
      renderer->renderText(score_blocks);
      renderer->renderText(win_lose);
      renderer->renderText(score_total);
      renderer->renderText(score_title);
    }
  }
}

void AngryBirds::moveCurrentBird()
{
  auto bird_width =
    birds[current_bird].obj.spriteComponent()->getSprite()->width();
  auto bird_height =
    birds[current_bird].obj.spriteComponent()->getSprite()->height();
  auto bird_xspeed = birds[current_bird].obj.getMove().x;
  auto bird_yspeed = birds[current_bird].obj.getMove().y;

  if (bird_held)
  {
    if (mouse_pos.distance(slingshot_origin) >= max_distance)
    {
      mouse_pos -= (slingshot_origin);
      mouse_pos.normalise();
      mouse_pos *= max_distance;
      mouse_pos += (slingshot_origin);
    }
    birds[current_bird].obj.setPos(
      mouse_pos.x - bird_width / 2, mouse_pos.y - bird_height / 2);
  }

  // If the last fired bird has stopped moving, prepare the next bird to fire
  if (birds[current_bird].getFired() && bird_xspeed == 0 && bird_yspeed == 0)
  {
    birds[current_bird].obj.setVisible(false);
    current_bird++;
    birds_left--;

    if (current_bird == MAX_BIRDS)
    {
      game_over    = true;
      current_bird = 0;
    }

    for (int i = 0; i < MAX_BIRDS; i++)
    {
      birds[i].obj.setPos(
        birds[i].obj.spriteComponent()->getSprite()->xPos() + 45,
        birds[i].obj.spriteComponent()->getSprite()->yPos());
    }

    auto bird_origin_xpos =
      (slingshot_origin.x -
       birds[current_bird].obj.spriteComponent()->getSprite()->width() / 2);
    auto bird_origin_ypos =
      (slingshot_origin.y -
       birds[current_bird].obj.spriteComponent()->getSprite()->height() / 2);

    birds[current_bird].obj.setPos(bird_origin_xpos, bird_origin_ypos);
  }
}
void AngryBirds::fireBird()
{
  float x_distance =
    (slingshot_origin.x - birds[current_bird].obj.getOrigin().x);
  float y_distance =
    (slingshot_origin.y - birds[current_bird].obj.getOrigin().y);

  birds[current_bird].obj.setMove(x_distance * 12, y_distance * 12);
  birds[current_bird].setFired(true);
}

void AngryBirds::birdsCollision(double dt_sec)
{
  float dt_float = static_cast<float>(dt_sec);

  for (int i = 0; i < MAX_BIRDS; i++)
  {
    auto bird_xspeed    = birds[i].obj.getMove().x;
    auto bird_yspeed    = birds[i].obj.getMove().y;
    auto bird_xpos      = birds[i].obj.spriteComponent()->getSprite()->xPos();
    auto bird_ypos      = birds[i].obj.spriteComponent()->getSprite()->yPos();
    auto bird_width     = birds[i].obj.spriteComponent()->getSprite()->width();
    auto bird_height    = birds[i].obj.spriteComponent()->getSprite()->height();
    auto bird_xmovement = birds[i].obj.getxMovement(dt_sec);
    auto bird_ymovement = birds[i].obj.getyMovement(dt_sec);

    // COME BACK HERE

    if (birds[i].obj.collisionCheckBounds(
          ground->getSpriteBounds(), bird_xmovement, bird_ymovement))
    {
      if (!(bird_xspeed > -10 && bird_xspeed < 10))
      {
        if (bird_xspeed > 0)
        {
          birds[i].obj.setMove(bird_xspeed - 500 * dt_float, -bird_yspeed / 5);
        }
        else if (bird_xspeed < 0)
        {
          birds[i].obj.setMove(bird_xspeed + 500 * dt_float, -bird_yspeed / 5);
        }
      }
      else
      {
        if (birds[i].obj.getMove().y > 10)
        {
          birds[i].obj.setMove(0, -bird_yspeed / 5);
        }
        else
        {
          birds[i].obj.setMove(0, 0);
        }
      }
      birds[i].setIgnoreGravity(true);
    }
    else
    {
      birds[i].setIgnoreGravity(false);
    }

    for (int j = 0; j < MAX_ENEMIES; j++)
    {
      if (
        birds[i].obj.collisionCheckBounds(
          enemies[j].obj.getSpriteBounds(), bird_xmovement, bird_ymovement) &&
        enemies[j].obj.getVisibile() && birds[i].obj.getVisibile())
      {
        enemies[j].obj.setMove(0, 0);
        enemies[j].obj.setVisible(false);
        enemies_left--;
        birds[i].obj.setMove(bird_xspeed / 3, bird_yspeed);
      }
    }

    for (int j = 0; j < MAX_BLOCKS; j++)
    {
      bool can_collide  = birds[i].getCollidePossible();
      bool ignore_block = birds[i].checkIgnoreBlock(j);
      auto block_xpos   = blocks[j].spriteComponent()->getSprite()->xPos();
      auto block_ypos   = blocks[j].spriteComponent()->getSprite()->yPos();
      auto block_width  = blocks[j].spriteComponent()->getSprite()->width();
      auto block_height = blocks[j].spriteComponent()->getSprite()->height();

      if (
        birds[i].obj.collisionCheckBounds(
          blocks[j].getSpriteBounds(), bird_xmovement, bird_ymovement) &&
        blocks[j].getVisibile())
      {
        if (birds[i].getActivatedPower() && birds[i].checkIgnoreBlock(100))
        {
          birds[i].setIgnoreBlock(j);
          can_collide = birds[i].checkIgnoreBlock(j);
        }

        if (blocks[j].getType() != 1)
        {
          if (can_collide && (birds[i].obj.getType() != 1))
          {
            blocks[j].setVisible(false);
            blocks_left--;
          }

          else if (birds[i].obj.getType() == 1)
          {
            if (!ignore_block && can_collide)
            {
              blocks[j].setVisible(false);
              blocks_left--;
            }
          }
        }

        if (
          (bird_xpos + bird_width < block_xpos ||
           bird_xpos > block_xpos + block_width) &&
          ((!can_collide && !ignore_block) || blocks[j].getType() == 1))
        {
          birds[i].obj.setMove(-bird_xspeed / 3, bird_yspeed);
        }
        else if (
          (bird_ypos + bird_height > block_ypos ||
           bird_ypos < block_ypos + block_height) &&
          ((!can_collide && !ignore_block)))
        {
          birds[i].obj.setMove(bird_xspeed / 3, -bird_yspeed / 3);
        }

        if (
          birds[i].obj.getType() != 1 ||
          (birds[i].obj.getType() == 1 && !ignore_block))
        {
          birds[i].increaseCollided();
        }

        if (!ignore_block)
        {
          birds[i].setIgnoreGravity(true);
        }
      }
    }
  }
}

void AngryBirds::enemyCollision(double dt_sec)
{
  for (int i = 0; i < MAX_ENEMIES; i++)
  {
    auto enemy_xmove  = enemies[i].obj.getxMovement(dt_sec);
    auto enemy_ymove  = enemies[i].obj.getyMovement(dt_sec);
    auto enemy_xspeed = enemies[i].obj.getMove().x;
    auto enemy_yspeed = enemies[i].obj.getMove().y;
    auto enemy_xpos   = enemies[i].obj.spriteComponent()->getSprite()->xPos();
    auto enemy_ypos   = enemies[i].obj.spriteComponent()->getSprite()->yPos();
    auto enemy_width  = enemies[i].obj.spriteComponent()->getSprite()->width();
    auto enemy_height = enemies[i].obj.spriteComponent()->getSprite()->height();

    enemies[i].obj.applyGravity(dt_sec);

    for (int j = 0; j < MAX_BLOCKS; j++)
    {
      auto block_xpos   = blocks[j].spriteComponent()->getSprite()->xPos();
      auto block_ypos   = blocks[j].spriteComponent()->getSprite()->yPos();
      auto block_width  = blocks[j].spriteComponent()->getSprite()->width();
      auto block_height = blocks[j].spriteComponent()->getSprite()->height();

      // Collision check between Enemy i and block j
      if (
        enemies[i].obj.collisionCheckBounds(
          blocks[j].getSpriteBounds(), enemy_xmove, enemy_ymove) &&
        blocks[j].getVisibile())
      {
        if (
          enemy_xpos + enemy_width < block_xpos ||
          enemy_xpos > block_xpos + block_width)
        {
          enemies[i].obj.setMove(-enemy_xspeed / 3, enemy_yspeed);
        }
        else if (
          enemy_ypos + enemy_height > block_ypos ||
          enemy_ypos < block_ypos + block_height)
        {
          enemies[i].obj.setMove(enemy_xspeed / 3, -enemy_yspeed / 3);
        }
      }
    }
    // Collision check between Enemy i and the ground
    if (enemies[i].obj.collisionCheckBounds(
          ground->getSpriteBounds(), enemy_xmove, enemy_ymove))
    {
      enemies[i].obj.setMove(enemy_xspeed / 3, -enemy_yspeed / 3);

      if (enemy_yspeed > -10 && enemy_yspeed < 10)
      {
        enemies[i].obj.setMove(0, 0);
      }
    }
  }
}

void AngryBirds::extraCollision(double dt_sec)
{
  // Re-usable variables for the Road Roller Object
  auto rr_xmove   = road_roller->getxMovement(dt_sec);
  auto rr_ymove   = road_roller->getyMovement(dt_sec);
  bool rr_visible = road_roller->getVisibile();

  for (int i = 0; i < MAX_ENEMIES; i++)
  {
    // Check Collision of Road Roller with Enemies
    if (
      rr_visible && enemies[i].obj.getVisibile() &&
      road_roller->collisionCheckBounds(
        enemies[i].obj.getSpriteBounds(), rr_xmove, rr_ymove))
    {
      enemies[i].obj.setMove(0, 0);
      enemies[i].obj.setVisible(false);
      enemies_left--;
    }
  }
  for (int i = 0; i < MAX_BLOCKS; i++)
  {
    // Check Collision of Road Roller with Blocks
    if (
      rr_visible && blocks[i].getVisibile() &&
      road_roller->collisionCheckBounds(
        blocks[i].getSpriteBounds(), rr_xmove, rr_ymove))
    {
      blocks[i].setMove(0, 0);
      blocks[i].setVisible(false);
      blocks_left--;
    }
  }

  // Check Collision of Road Roller with Ground
  if (road_roller->collisionCheckBounds(
        ground->getSpriteBounds(), rr_xmove, rr_ymove))
  {
    road_roller->setMove(0, 0);
  }
}

bool AngryBirds::initBirds()
{
  for (int i = 0; i < MAX_BIRDS; i++)
  {
    birds[i].obj.setType(i);

    if (birds[i].obj.getType() == 0)
    {
      if (!birds[i].obj.addSpriteComponent(
            renderer.get(),
            "data/images/kenney_animalpackredux/PNG/Round "
            "(outline)/parrot.png"))
      {
        return false;
      }
    }

    else if (birds[i].obj.getType() == 1)
    {
      if (!birds[i].obj.addSpriteComponent(
            renderer.get(),
            "data/images/kenney_animalpackredux/PNG/Round "
            "(outline)/penguin.png"))
      {
        return false;
      }
      birds[i].setMaxCollides(0);
    }
    else if (birds[i].obj.getType() == 2)
    {
      if (!birds[i].obj.addSpriteComponent(
            renderer.get(),
            "data/images/kenney_animalpackredux/PNG/Round (outline)/duck.png"))
      {
        return false;
      }
    }
    else if (birds[i].obj.getType() == 3)
    {
      if (!birds[i].obj.addSpriteComponent(
            renderer.get(),
            "data/images/kenney_animalpackredux/PNG/Round (outline)/chick.png"))
      {
        return false;
      }
    }
    else if (birds[i].obj.getType() == 4)
    {
      if (!birds[i].obj.addSpriteComponent(
            renderer.get(),
            "data/images/kenney_animalpackredux/PNG/Round (outline)/owl.png"))
      {
        return false;
      }
    }
    else if (birds[i].obj.getType() == 5)
    {
      if (!birds[i].obj.addSpriteComponent(
            renderer.get(),
            "data/images/kenney_animalpackredux/PNG/Round "
            "(outline)/chicken.png"))
      {
        return false;
      }
    }

    birds[i].obj.setxSpacing(45 * static_cast<float>(i));
    birds[i].obj.spriteComponent()->getSprite()->width(35);
    birds[i].obj.spriteComponent()->getSprite()->height(35);

    float bird_height   = birds[i].obj.spriteComponent()->getSprite()->height();
    float window_height = static_cast<float>(ASGE::SETTINGS.window_height);
    float bird_xspacing = birds[i].obj.getxSpacing();

    birds[i].obj.setPos(320 - bird_xspacing, window_height - bird_height - 110);
    birds[i].reset();
  }

  float firing_start_xpos =
    (slingshot_origin.x -
     birds[current_bird].obj.spriteComponent()->getSprite()->width() / 2);
  float firing_start_ypos =
    (slingshot_origin.y -
     birds[current_bird].obj.spriteComponent()->getSprite()->height() / 2);

  birds[current_bird].obj.setPos(firing_start_xpos, firing_start_ypos);

  return true;
}

bool AngryBirds::initEnemies()
{
  for (int i = 0; i < MAX_ENEMIES; i++)
  {
    if (!enemies[i].obj.addSpriteComponent(
          renderer.get(),
          "data/images/kenney_animalpackredux/PNG/Round (outline)/pig.png"))
    {
      return false;
    }
    enemies[i].obj.spriteComponent()->getSprite()->setGlobalZOrder(
      static_cast<short>(i));
    enemies[i].reset();
  }
  enemies[0].obj.setPos(860, 480);
  enemies[1].obj.setPos(940, 480);
  enemies[2].obj.setPos(900, 320);
  enemies[3].obj.setPos(1100, 360);
  enemies[4].obj.setPos(900, 200);

  return true;
}

bool AngryBirds::initBlocks()
{
  for (int i = 0; i < MAX_BLOCKS; i++)
  {
    if (i <= 2)
    {
      if (!blocks[i].addSpriteComponent(
            renderer.get(),
            "data/images/kenney_physicspack/PNG/Metal "
            "elements/elementMetal011.png"))
      {
        return false;
      }
      blocks[i].spriteComponent()->getSprite()->width(40);
      blocks[i].spriteComponent()->getSprite()->height(40);
      blocks[i].setType(1);
    }

    if (i <= 5 && i > 2)
    {
      if (!blocks[i].addSpriteComponent(
            renderer.get(),
            "data/images/kenney_physicspack/PNG/Wood "
            "elements/elementWood012.png"))
      {
        return false;
      }
      blocks[i].spriteComponent()->getSprite()->width(120);
      blocks[i].spriteComponent()->getSprite()->height(40);
      blocks[i].setType(2);
    }

    if (i <= 7 && i > 5)
    {
      if (!blocks[i].addSpriteComponent(
            renderer.get(),
            "data/images/kenney_physicspack/PNG/Wood "
            "elements/elementWood019.png"))
      {
        return false;
      }
      blocks[i].spriteComponent()->getSprite()->width(40);
      blocks[i].spriteComponent()->getSprite()->height(120);
      blocks[i].setType(2);
    }
    if (i <= 8 && i > 7)
    {
      if (!blocks[i].addSpriteComponent(
            renderer.get(),
            "data/images/kenney_physicspack/PNG/Metal "
            "elements/elementMetal020.png"))
      {
        return false;
      }
      blocks[i].spriteComponent()->getSprite()->width(40);
      blocks[i].spriteComponent()->getSprite()->height(180);
      blocks[i].setType(1);
    }
    if (i < MAX_BLOCKS && i > 8)
    {
      if (!blocks[i].addSpriteComponent(
            renderer.get(),
            "data/images/kenney_physicspack/PNG/Metal "
            "elements/elementMetal027.png"))
      {
        return false;
      }
      blocks[i].spriteComponent()->getSprite()->width(40);
      blocks[i].spriteComponent()->getSprite()->height(40);
      blocks[i].setType(1);
    }

    blocks[i].setMove(0, 0);
    blocks[i].setVisible(true);
    blocks[i].setSpeed(100);
  }
  blocks[5].spriteComponent()->getSprite()->width(240);
  blocks[0].setPos(800, 560);
  blocks[1].setPos(900, 560);
  blocks[2].setPos(1000, 560);
  blocks[3].setPos(800, 520);
  blocks[4].setPos(920, 520);
  blocks[5].setPos(800, 360);
  blocks[6].setPos(800, 400);
  blocks[7].setPos(1000, 400);
  blocks[8].setPos(1100, 420);
  blocks[9].setPos(900, 250);

  return true;
}

bool AngryBirds::initScene()
{
  // Set up Slingshot Object
  if (!slingshot->addSpriteComponent(renderer.get(), "data/images/tempss.png"))
  {
    return false;
  }
  slingshot->spriteComponent()->getSprite()->width(60);
  slingshot->spriteComponent()->getSprite()->height(120);
  slingshot->spriteComponent()->getSprite()->yPos(
    static_cast<float>(ASGE::SETTINGS.window_height) -
    slingshot->spriteComponent()->getSprite()->height() - 110);
  slingshot->spriteComponent()->getSprite()->xPos(300);

  // Set up Road Roller Object
  if (!road_roller->addSpriteComponent(
        renderer.get(), "data/images/road roller.png"))
  {
    return false;
  }
  road_roller->spriteComponent()->getSprite()->width(150);
  road_roller->spriteComponent()->getSprite()->height(100);
  road_roller->spriteComponent()->getSprite()->yPos(-200);
  road_roller->spriteComponent()->getSprite()->xPos(-200);
  road_roller->setMove(0, 0);

  // Set up Ground Object
  if (!ground->addSpriteComponent(
        renderer.get(), "data/images/kenney_physicspack/PNG/Other/grass.png"))
  {
    return false;
  }
  float window_width  = static_cast<float>(ASGE::SETTINGS.window_width);
  float window_height = static_cast<float>(ASGE::SETTINGS.window_height);
  ground->spriteComponent()->getSprite()->width(window_width);
  ground->spriteComponent()->getSprite()->height(120);
  ground->spriteComponent()->getSprite()->yPos(
    window_height - slingshot->spriteComponent()->getSprite()->height());
  ground->spriteComponent()->getSprite()->xPos(0);

  level_background = renderer->createRawSprite();
  level_background->loadTexture("data/images/lvl1.png");
  level_background->width(window_width);
  level_background->height(window_height);
  level_background->setGlobalZOrder(-15);

  return true;
}

void AngryBirds::adjustBirds()
{
  for (int i = 0; i < MAX_BIRDS; i++)
  {
    if (birds[i].obj.getType() == 1 && birds[i].getActivatedPower())
    {
      birds[i].obj.spriteComponent()->getSprite()->loadTexture("data/images/"
                                                               "kenney_"
                                                               "animalpackredux"
                                                               "/PNG/Round "
                                                               "(outline)/"
                                                               "penguinPowered."
                                                               "png");
      birds[i].obj.spriteComponent()->getSprite()->width(50);
      birds[i].obj.spriteComponent()->getSprite()->height(50);
    }
    if (
      birds[i].obj.getType() == 5 && birds[i].getActivatedPower() &&
      birds[i].obj.spriteComponent()->getSprite()->yPos() < -100)
    {
      road_roller->setVisible(true);
      birds[i].obj.setMove(0, 10);
      road_roller->setPos(
        birds[i].obj.spriteComponent()->getSprite()->xPos(),
        birds[i].obj.spriteComponent()->getSprite()->yPos());
      road_roller->setMove(0, 10);
    }
  }
}

void AngryBirds::extraMovement(double dt_sec)
{
  // Movement for additional objects
  float rr_xpos     = road_roller->spriteComponent()->getSprite()->xPos();
  float rr_ypos     = road_roller->spriteComponent()->getSprite()->yPos();
  double rr_ymove   = road_roller->getMove().y * dt_sec;
  float rr_new_ypos = static_cast<float>(rr_ypos + rr_ymove);

  road_roller->setPos(rr_xpos, rr_new_ypos);
  road_roller->applyGravity(dt_sec);
}

void AngryBirds::endGame()
{
  if (enemies_left == 0)
  {
    win_lose_string = "win!";
  }

  else
  {
    win_lose_string = "lose!";
  }
}
