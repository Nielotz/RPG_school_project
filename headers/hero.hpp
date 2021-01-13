#pragma once


#include "entity.hpp"
#include "item.hpp"
#include <string>

struct Hero : Entity
{
    // In tiles per second.
    float move_speed = 5.;

    ProfessionType profession_type = ProfessionType::NONE;
    std::string profession_type_name;

    //Inventory
    Item inventory[20];

    ///Threads status in the plot
    int8_t thread_status[10];

    Hero() = default;

    /// Increase lvl and update hero stats.
    void increase_lvl();

    //Update Level-dependent attributes
    void update_attributes();
};
