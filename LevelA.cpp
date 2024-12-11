#include "LevelA.h"
#include "Utility.h"
#include <vector>

#define FIXED_TIMESTEP 0.0166666f
#define FOOD_COUNT 1

#define ENEMY_COUNT 3

#define LEVEL_WIDTH 12
#define LEVEL_HEIGHT 9

extern int g_lives;
unsigned int LEVELA_DATA[] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1,
    1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1,
    1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1,
    1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
};


LevelA::~LevelA()
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

void LevelA::initialise() {
    m_game_state.next_scene_id = -1;

    m_game_state.eat_sound = Mix_LoadWAV("assets/audio/bite.wav");
    m_game_state.lose_sfx = Mix_LoadWAV("assets/audio/lose.wav");
    m_game_state.boom_sfx = Mix_LoadWAV("assets/audio/boom.wav");
    m_game_state.jump_sfx = Mix_LoadWAV("assets/audio/jump.wav");

    if (!m_game_state.eat_sound) {
        std::cerr << "Failed to load eat sound: " << Mix_GetError() << std::endl;
    }

    // Load music
    m_game_state.bgm = Mix_LoadMUS("assets/audio/CloudDancer.mp3");
    if (!m_game_state.bgm) {
        std::cerr << "Failed to load background music: " << Mix_GetError() << std::endl;
    }

    // Play music
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);  // Ensure audio system is initialized
    Mix_PlayMusic(m_game_state.bgm, -1);                // Play the background music
    Mix_VolumeMusic(MIX_MAX_VOLUME / 16.0f);            // Adjust music volume

    
    m_game_state.background_texture_id = Utility::load_texture("assets/images/background.png");
    GLuint map_texture_id = Utility::load_texture("assets/images/pink-tileset.png");

    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVELA_DATA, map_texture_id, 0.9f, 4, 1);
    
    GLuint player_texture_id = Utility::load_texture("assets/images/char.png");
    int player_walking_animation[4][4] = {
        {1, 2, 3, 4},
        {5, 6, 7, 8},
        {9, 10, 11, 12},
        {13, 14, 15, 16}
    };
    
    float tileSize = m_game_state.map->get_tile_size();
    std::cout << tileSize<<std::endl;

    m_game_state.player = new Entity(player_texture_id, 2.0f, glm::vec3(0.0f), 1.0f,
                                    player_walking_animation, 0.2f, 4, 0, 4, 4, 0.5f, 0.5f, PLAYER);
    
    GLuint enemy_texture_id = Utility::load_texture("assets/images/enemy-fruit-fly.png");
    m_game_state.player->set_jumping_power(2.5f);
    int batpig_fly_animation[1][4] = {
        { 0, 1, 2, 3}
    };
    
    GLuint food_texture_id = Utility::load_texture("assets/images/jelloporter-pink.png");

    m_game_state.enemies = new Entity[ENEMY_COUNT];

    m_game_state.enemies[0] = Entity(enemy_texture_id, 1.0f, glm::vec3(0.0f), 0.0f, batpig_fly_animation, 0.0f, 3, 0, 3, 1, 0.9f, 0.9f, ENEMY);
    m_game_state.enemies[0].set_ai_type(FLYER1);
    m_game_state.enemies[0].activate();
    
    m_game_state.enemies[1] = Entity(enemy_texture_id, 1.0f, glm::vec3(0.0f), 0.0f, batpig_fly_animation, 0.0f, 3, 0, 3, 1, 0.9f, 0.9f, ENEMY);
    m_game_state.enemies[1].set_ai_type(FLYER2);
    m_game_state.enemies[1].activate();
   
    m_game_state.enemies[2] = Entity(enemy_texture_id, 1.0f, glm::vec3(0.0f), 0.0f, batpig_fly_animation, 0.0f, 3, 0, 3, 1, 0.9f, 0.9f, ENEMY);
    m_game_state.enemies[2].set_ai_type(FLYER3);
    m_game_state.enemies[2].activate();
    
    m_game_state.food = new Entity[FOOD_COUNT];
    m_game_state.food[0] = Entity(food_texture_id, 0.0f, glm::vec3(0.0f), 0.0f, nullptr, 0.0f, 0, 0, 1, 1, 0.15f, 0.15f, FOOD);
    m_game_state.food[0].set_position(glm::vec3(4.5f, -1.0f, 0.0f));
//    
//    m_game_state.food[0].set_position(glm::vec3(9.0f, -1.0f, 0.0f));
//    m_game_state.food[1] = Entity(food_texture_id, 0.0f, glm::vec3(0.0f), 0.0f, nullptr, 0.0f, 0, 0, 1, 1, 0.15f, 0.15f, FOOD);
//    m_game_state.food[1].set_position(glm::vec3(3.6f, -5.4f, 0.0f));
//    m_game_state.food[2] = Entity(food_texture_id, 0.0f, glm::vec3(0.0f), 0.0f, nullptr, 0.0f, 0, 0, 1, 1, 0.15f, 0.15f, FOOD);
//    m_game_state.food[2].set_position(glm::vec3(9.0f, -1.0f, 0.0f));
//
//   
    m_game_state.player->set_position(glm::vec3(1.0f, -1.0f, 0.0f));
    
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

void LevelA::update(float delta_time) {
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

void LevelA::render(ShaderProgram *program) {
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
