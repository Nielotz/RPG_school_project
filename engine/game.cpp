#include <iostream>

#include "../headers/game.hpp"
#include "../headers/consts.hpp"
#include "../headers/enemy.hpp"

using std::clog;
using std::cout;
using std::endl;


void Game::game_loop()
{
    load_map();

    // TEMPLATE OF ADDNING AN ENEMY.
    Enemy enemy; // Change to any enemy class, when Magda adds them.
    // enemy.name should be set in enemy class
    enemies.push_back(enemy);

    enemy.position = Position(profile.hero.position.x, profile.hero.position.y + 2);
    std::string texture_path;
    if (enemy.name == "")
        texture_path = PATH::ENTITY_TEXTURES::BEAR::MAP;
    else if (enemy.name == "xd")
        texture_path = "xd";
    graphic.load_entity_texture(enemy, texture_path);

    graphic.set_entity_position(enemy, enemy.position);

    graphic.set_entity_size(enemy, 2);
    ///////////////////////////////////////////////////


    while (graphic.window->isOpen())
    {
        control.parse_walk_events(graphic, *this);

        graphic.update();
        graphic.window->display();
    }
    profile.save();
}


Game::Game() :
    graphic(Graphic(profile.hero))
{
    start();
    clog << "Initializing graphics..." << endl;
    game_loop();
};


void Game::start()
{
    cout << "Starting game." << endl;

    clog << "Choosing profile... " << profile.name << endl;
    choose_profile(profile);
    cout << "Chosen profile: " << profile.name << endl;


    clog << "Loading profile..." << endl;
    profile.load();
    clog << "Loaded profile, nick: " << profile.hero.name << endl;
    //clog << "lvl: "<< profile.hero.lvl<<"\nexp: " << profile.hero.exp << "\nhp: " << profile.hero.hp << "\nhp_max: " << profile.hero.hp_max << "\nmana: " << profile.hero.mana << "\nstamina: " << profile.hero.stamina << "\nstrength: " << profile.hero.strength << "\nagility: " << profile.hero.agility << "\nintelect: " << profile.hero.intelect << "\nblock: " << profile.hero.block << "\ndefence: " << profile.hero.defence << "\nAT: " << profile.hero.attack_time << "\n";

}

void Game::choose_profile(Profile& profile)
{
    profile.path = "test_profile.txt";
}

void Game::exit()
{
    graphic.window->close();
}

void Game::load_map()
{
    std::string map_file;
    if (profile.hero.map == 0)
        map_file = PATH::MAP::TEXTURES::TEST;
    else if (profile.hero.map == 1)
        map_file = PATH::MAP::TEXTURES::START;
    else if (profile.hero.map == 2)
        map_file = PATH::MAP::TEXTURES::FOREST;


    map.load_texture(map_file);
    map.load_map_collisions(map_file);


    graphic.load_level(map);

    graphic.load_hero_textures(profile.hero, PATH::HERO::TEXTURES::warrior);

    graphic.set_hero_position(profile.hero.position);
    graphic.update_view();
    graphic.update_view();
    graphic.update_view();
    graphic.update_view();
    graphic.update_view();
    graphic.update_view();
}

void Game::move_hero(const Direction& direction)
{
    if (graphic.is_hero_moving)
        return;
    if (can_hero_move(direction))
    {
        if (direction == Direction::LEFT)
        {
            graphic.move_hero(Direction::LEFT);
            profile.hero.position.x--;
        }
        else if (direction == Direction::RIGHT)
        {
            graphic.move_hero(Direction::RIGHT);
            profile.hero.position.x++;
        }
        else if (direction == Direction::TOP)
        {
            graphic.move_hero(Direction::TOP);
            profile.hero.position.y--;
        }
        else // BOTTOM
        {
            graphic.move_hero(Direction::BOTTOM);
            profile.hero.position.y++;
        }
    }
}

bool Game::can_hero_move(const Direction& direction)
{
    int32_t cords[2] = { 0,0 }; //y,x
    if (direction == Direction::LEFT)
    {
        cords[1] = -1;
    }
    else if (direction == Direction::RIGHT)
    {
        cords[1] = 1;
    }
    else if (direction == Direction::TOP)
    {
        cords[0] = -1;
    }
    else // BOTTOM
    {
        cords[0] = 1;
    }
    if (map.collisions[(int32_t)(profile.hero.position.y + cords[0])][(int32_t)(profile.hero.position.x + cords[1])] == ' ')
        return true;
    else
        return false;
}
