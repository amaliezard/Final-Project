#include "LevelB.h"
#include "Utility.h"
#include <vector>

#define FIXED_TIMESTEP 0.0166666f

#define ENEMY_COUNT 3
#define FOOD_COUNT 4
extern int g_lives;

#define LEVEL_WIDTH 12
#define LEVEL_HEIGHT 9

unsigned int LEVELB_DATA[] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1,
    1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1,
    1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
    1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
};



LevelB::~LevelB()
{
    delete m_game_state.player;
    delete m_game_state.map;
    Mix_FreeChunk(m_game_state.eat_sound);
    Mix_FreeChunk(m_game_state.lose_sfx);
    Mix_FreeChunk(m_game_state.boom_sfx);
    Mix_FreeMusic(m_game_state.bgm);
    m_game_state.eat_sound = nullptr;
    m_game_state.lose_sfx = nullptr;
    m_game_state.boom_sfx = nullptr;
    m_game_state.bgm = nullptr;
}

void LevelB::initialise() {
    m_game_state.next_scene_id = -1;

    GLuint map_texture_id = Utility::load_texture("assets/images/pink-tileset.png");
    m_game_state.eat_sound = Mix_LoadWAV("assets/audio/bite.wav");
    m_game_state.lose_sfx = Mix_LoadWAV("assets/audio/lose.wav");

    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVELB_DATA, map_texture_id, 0.9f, 4, 1);
    
    GLuint player_texture_id = Utility::load_texture("assets/images/char.png");
    int player_walking_animation[4][4] = {
        {1, 2, 3, 4},
        {5, 6, 7, 8},
        {9, 10, 11, 12},
        {13, 14, 15, 16}
    };
    
    float tileSize = m_game_state.map->get_tile_size();
    std::cout << tileSize<<std::endl;
    m_game_state.boom_sfx = Mix_LoadWAV("assets/audio/boom.wav");
    m_game_state.player = new Entity(player_texture_id, 2.0f, glm::vec3(0.0f), 1.0f,
                                    player_walking_animation, 0.2f, 4, 0, 4, 4, 0.7f, 0.7f, PLAYER);
    
    GLuint enemy_texture_id = Utility::load_texture("assets/images/enemy-eye-sickle.png");
    m_game_state.player->set_jumping_power(2.5f);
    int eye_sickle_animation[1][4] = {
        { 0, 1, 2, 3}
    };
    
    GLuint food_texture_id = Utility::load_texture("assets/images/jelloporter-green.png");
    
    m_game_state.enemies = new Entity[ENEMY_COUNT];

    for (int i = 0; i < ENEMY_COUNT; i++) {
        m_game_state.enemies[i] = Entity( enemy_texture_id, 1.2f, glm::vec3(0.0f), 0.0f,eye_sickle_animation, 0.0f, 4, 0, 4, 1, 0.3f, 0.3f, ENEMY );
        m_game_state.enemies[i].activate();
    }

    m_game_state.enemies[0].set_position(glm::vec3(5.4f, -0.7f, 0.0f));
    m_game_state.enemies[0].set_ai_type(OSCILLATE2);
    m_game_state.enemies[0].oscillation_timer = 0.1f;

    m_game_state.enemies[1].set_position(glm::vec3(2.8f, -1.7f, 0.0f));
    m_game_state.enemies[1].set_ai_type(JUMPER);
    m_game_state.enemies[1].oscillation_timer = 0.2f;

    m_game_state.enemies[2].set_position(glm::vec3(0.8f, -4.3f, 0.0f));
    m_game_state.enemies[2].set_ai_type(OSCILLATE);
    m_game_state.enemies[2].oscillation_timer = 0.3f;

    m_game_state.player->set_position(glm::vec3(4.5f, -0.9f, 0.0f));
    
    m_game_state.food = new Entity[FOOD_COUNT];
    m_game_state.food[0] = Entity(food_texture_id, 0.0f, glm::vec3(0.0f), 0.0f, nullptr, 0.0f, 0, 0, 1, 1, 0.15f, 0.15f, FOOD);
    m_game_state.food[0].set_position(glm::vec3(0.9f, -1.5f, 0.0f));
    m_game_state.food[1] = Entity(food_texture_id, 0.0f, glm::vec3(0.0f), 0.0f, nullptr, 0.0f, 0, 0, 1, 1, 0.15f, 0.15f, FOOD);
    m_game_state.food[1].set_position(glm::vec3(1.5f, -6.3f, 0.0f));
    m_game_state.food[2] = Entity(food_texture_id, 0.0f, glm::vec3(0.0f), 0.0f, nullptr, 0.0f, 0, 0, 1, 1, 0.15f, 0.15f, FOOD);
    m_game_state.food[2].set_position(glm::vec3(9.0f, -1.0f, 0.0f));
    m_game_state.food[3] = Entity(food_texture_id, 0.0f, glm::vec3(0.0f), 0.0f, nullptr, 0.0f, 0, 0, 1, 1, 0.15f, 0.15f, FOOD);
    m_game_state.food[3].set_position(glm::vec3(8.0f, -4.4f, 0.0f));

    // ————— BLENDING ————— //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Setup background music and sound effects
    
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    Mix_PlayMusic(m_game_state.bgm, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 16.0f);
    
    m_game_state.jump_sfx = Mix_LoadWAV("assets/audio/jump.wav");
    m_game_state.bgm = Mix_LoadMUS("assets/audio/CloudDancer.mp3");
    Mix_PlayMusic(m_game_state.bgm, -1);
    
}

void LevelB::update(float delta_time) {
    m_game_state.player->update(delta_time, m_game_state.player, nullptr, 0, m_game_state.map);

    for (int i = 0; i < ENEMY_COUNT; i++) {
            if (!m_game_state.enemies[i].is_active()) continue;

            m_game_state.enemies[i].update(delta_time, m_game_state.player, nullptr, 0, m_game_state.map);

            if (m_game_state.player->check_collision(&m_game_state.enemies[i])) {
                if (m_game_state.player->get_velocity().y < 0) {
                    m_game_state.enemies[i].deactivate();
                } else {
                    g_lives--;
                    if (g_lives > 0) {
                        initialise();
                    } else {
                        if (m_game_state.lose_sfx) {
                            Mix_PlayChannel(-1, m_game_state.lose_sfx, 0);
                        }
                        m_game_state.next_scene_id = -1;
                    }
                    return;
                }
            }
        }


    int collected_food_count = 0;
    for (int i = 0; i < FOOD_COUNT; i++) {
        if (m_game_state.food[i].is_active()) {
            if (m_game_state.player->check_collision(&m_game_state.food[i])) {
                m_game_state.food[i].deactivate();
                if (m_game_state.eat_sound) {
                    Mix_PlayChannel(-1, m_game_state.eat_sound, 0);
                }
            }
        } else {
            collected_food_count++;
        }
    }

    if (collected_food_count == FOOD_COUNT) {
        m_game_state.next_scene_id = 3;
    }
}

void LevelB::render(ShaderProgram *program) {
    m_game_state.map->render(program);
    m_game_state.player->render(program);
    glBindTexture(GL_TEXTURE_2D, m_game_state.background_texture_id);

    for (int i = 0; i < ENEMY_COUNT; i++) {
        if (m_game_state.enemies[i].is_active()) {
            m_game_state.enemies[i].render(program);
        }

    }
    for (int i = 0; i < FOOD_COUNT; i++) {
        if (m_game_state.food[i].is_active()) {
            m_game_state.food[i].render(program);
        }
    }
    
   if (g_lives <= 0) {
       glm::vec3 player_position = m_game_state.player->get_position();
       Utility::draw_text(program, Utility::load_texture("assets/fonts/font1.png"), "YOU LOSE", 0.6f, 0.1f, glm::vec3(player_position.x - 2.5f, player_position.y + 0.5f, 0.0f));
       SDL_GL_SwapWindow(SDL_GL_GetCurrentWindow());
       SDL_Delay(3000);
       exit(0);
   }
      
}
