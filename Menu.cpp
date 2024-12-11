#include "Menu.h"
#include "Utility.h"
#include <vector>
#define LEVEL1_WIDTH 20
#define LEVEL1_HEIGHT 5

//#define LEVEL_HEIGHT 5

//unsigned int MENU_DATA[] =
//{
//    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
//};


Menu::~Menu()
{
    delete m_game_state.player;
    delete m_game_state.map;
    Mix_FreeChunk(m_game_state.jump_sfx);
    Mix_FreeMusic(m_game_state.bgm);
}

void Menu::initialise()
{
    m_game_state.next_scene_id = -1;
}

void Menu::update(float delta_time)
{
}

void Menu::render(ShaderProgram *program)
{
  
    GLuint font_texture = Utility::load_texture("assets/images/font1.png");
    glm::mat4 model_matrix = glm::mat4(1.0f);
    program->set_model_matrix(model_matrix);

    glClear(GL_COLOR_BUFFER_BIT);
    
    Utility::draw_text(program, font_texture, "Welcome to my game!", 0.75f, -0.3f, glm::vec3(1.0f, -1.5f, 0.0f));
    Utility::draw_text(program, font_texture, "Press ENTER to Start", 0.5f, -0.25f, glm::vec3(2.5f,-2.5f, 0.0f));
    Utility::draw_text(program, font_texture, "Press K or Crush to Kill Enemy", 0.5f, -0.25f, glm::vec3(1.5f,-3.5f, 0.0f));
    Utility::draw_text(program, font_texture, "YOU HAVE 3 LIVES", 0.5f, -0.25f, glm::vec3(2.9f,-4.5f, 0.0f));
;
}
    


  
