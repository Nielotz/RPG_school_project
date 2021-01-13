#include <SFML/Graphics.hpp>

#include <iostream>

#include "../headers/graphic.hpp"
#include "../headers/consts.hpp"


void Graphic::update_hero_animation_frame()
{
	std::chrono::duration<double> time_diff = std::chrono::steady_clock::now() - time_point_of_last_change_animation_frame;

	if (time_diff.count() >= MOVE_ANIMATION_FRAME_TIME)
	{
		time_point_of_last_change_animation_frame = std::chrono::steady_clock::now();
		next_hero_animation_frame();
		update_hero_texture_position();
	}
}

void Graphic::update_hero_step()
{
	std::chrono::duration<double> time_diff = std::chrono::steady_clock::now() - time_point_of_last_move_hero;

	if (move_step == 0 || time_diff.count() >= MOVE_STEP_FRAME_TIME)
	{
		if (MOVE_STEPS == move_step)
		{
			if (distance_left_to_move > 0.0000001)
				update_hero_texture_position();
			is_hero_moving = false;
			hero_animation_frame = 0;
		}
		else
		{
			distance_left_to_move -= float(distance_left_to_move) / float((MOVE_STEPS - move_step++));
			// std::cout << "UPDATED ANIMATION MOVE STEP TO: " << int(move_step) << std::endl;

			update_hero_texture_position();
		}


		time_point_of_last_move_hero = std::chrono::steady_clock::now();
	}
}

Graphic::Graphic(Hero& hero)
	:MOVE_STEP_FRAME_TIME(1. / (MOVE_STEPS * hero.move_speed))
{
	time_point_of_last_change_animation_frame = time_point_of_last_move_hero
		= std::chrono::steady_clock::now();

	this->hero = &hero;
	this->window = std::make_unique<sf::RenderWindow>(
		sf::VideoMode(CONSTS::GAME_WINDOW_SIZE.x, CONSTS::GAME_WINDOW_SIZE.y), "Metal chick solid");

	map_view.setSize(sf::Vector2f(
		float(CONSTS::PLAYER_VIEW_RANGE.x * CONSTS::TILE_SIZE),
		float(CONSTS::PLAYER_VIEW_RANGE.y * CONSTS::TILE_SIZE)
	));

	map_view.setCenter(sf::Vector2f(
		float(CONSTS::PLAYER_VIEW_RANGE.x * ((CONSTS::TILE_SIZE + 1) / 2)),
		float(CONSTS::PLAYER_VIEW_RANGE.y * ((CONSTS::TILE_SIZE + 1) / 2))
	));

	// setViewport scales view!
	this->map_view.setViewport(sf::FloatRect(0.f, 0.f, 10.f / 16.f, 1.f));
}

void Graphic::load_level(Map& map)
{
	this->map = map;
	this->map.sprite.setScale(float(CONSTS::SCALE), float(CONSTS::SCALE));

	this->map.sprite.setPosition(0, 0); // To be read from map_data.
}

void Graphic::load_texture(Entity& entity, std::string& path)
{
	sf::Texture texture;
	if (!texture.loadFromFile(path))
		throw std::runtime_error("Cannot load " + path);

	texture.setSmooth(true);

	sf::Sprite sprite(texture);

	entity_sprites.insert(std::pair<uint64_t, sf::Sprite>(entity.id, sprite));
}

void Graphic::load_hero_textures(Hero& entity,
	const std::unordered_map<Direction, std::vector<std::string>> paths_to_hero_textures)
{
	typedef std::pair<sf::Sprite, sf::Texture> sprite_texture_pair;
	typedef std::vector<sprite_texture_pair> sprite_texture_vector_of_pairs;
	hero_sprites_with_texture.clear();

	for (const auto& direction : { Direction::LEFT, Direction::RIGHT, Direction::BOTTOM, Direction::TOP })
	{
		hero_sprites_with_texture.insert({ direction, {} });
		sprite_texture_vector_of_pairs& directional_hero_sprites_with_texture = hero_sprites_with_texture.at(direction);

		directional_hero_sprites_with_texture.resize(paths_to_hero_textures.at(direction).size());

		uint16_t texture_id = 0;
		for (const std::string& path : paths_to_hero_textures.at(direction))
		{
			directional_hero_sprites_with_texture[texture_id] = sprite_texture_pair{ sf::Sprite(), sf::Texture() };

			sf::Sprite& sprite = directional_hero_sprites_with_texture[texture_id].first;
			sf::Texture& texture = directional_hero_sprites_with_texture[texture_id].second;

			if (!texture.loadFromFile(path))
				throw std::runtime_error("Cannot load " + path);

			texture.setSmooth(true);

			sprite.setTexture(texture);

			float scale = float(CONSTS::TILE_SIZE) / float(texture.getSize().x);
			sprite.setScale(sf::Vector2f{ scale, scale });

			sprite.setPosition(position_to_display_position(hero->position, sprite));

			hero_sprites_with_texture.insert(
				{ direction, sprite_texture_vector_of_pairs{ sprite_texture_pair {sprite, texture} } }
			);

			texture_id++;
		}
	}
}

void Graphic::set_hero_position(Position& position)
{
	hero_looking_direction = Direction::BOTTOM;

	sf::Sprite& hero_sprite = hero_sprites_with_texture.at(hero_looking_direction)[hero_animation_frame].first;
	hero_sprite.setPosition(position_to_display_position(position, hero_sprite));
}

void Graphic::draw_entities()

{
	for (auto& texture : entity_sprites)
		if (true)
			window->draw(entity_sprites[1]);
}

void Graphic::draw_hero()
{
	typedef std::pair<sf::Sprite, sf::Texture> sprite_texture_pair;
	typedef std::vector<sprite_texture_pair> sprite_texture_vector_of_pairs;

	// Directional textures.
	sprite_texture_vector_of_pairs& animation_frames = hero_sprites_with_texture.at(hero_looking_direction);

	sf::Sprite& hero_sprite = animation_frames[hero_animation_frame].first;
	update_hero_texture_position();

	window->draw(hero_sprite);
}

void Graphic::update()
{
	update_hero();

	draw_map();
	//draw_entities();
	draw_hero();
}

void Graphic::move_view(const Direction& direction)
{
	// Margin for float imprecision included.
	if (direction == Direction::LEFT
		&& this->map_view.getCenter().x + this->map_view.getSize().x / 2 < CONSTS::TILE_SIZE * map.tiles_number.x - 3)
		map_view.move(float(CONSTS::TILE_SIZE), 0);

	if (direction == Direction::RIGHT
		&& this->map_view.getCenter().x - this->map_view.getSize().x / 2 > 3)
		map_view.move(float(-CONSTS::TILE_SIZE), 0);

	if (direction == Direction::TOP
		&& this->map_view.getCenter().y + this->map_view.getSize().y / 2 < CONSTS::TILE_SIZE * map.tiles_number.y - 3)
		map_view.move(0, float(CONSTS::TILE_SIZE));

	if (direction == Direction::BOTTOM
		&& this->map_view.getCenter().y - this->map_view.getSize().y / 2 > 3)
		map_view.move(0, float(-CONSTS::TILE_SIZE));
}

// Update the hero position on the map.
// Updates view.
void Graphic::update_hero_texture_position()
{
	sf::Sprite& hero_sprite = hero_sprites_with_texture.at(hero_looking_direction)[hero_animation_frame].first;

	sf::Vector2f hero_pos = position_to_display_position(hero->position, hero_sprite);

	sf::Vector2f new_pos(hero_pos);

	float distance_traveled = -distance_left_to_move;

	if (hero_looking_direction == Direction::LEFT)
		new_pos.x -= distance_traveled;
	else if (hero_looking_direction == Direction::RIGHT)
		new_pos.x += distance_traveled;
	else if (hero_looking_direction == Direction::TOP)
		new_pos.y -= distance_traveled;
	else // BOTTOM
		new_pos.y += distance_traveled;

	hero_sprite.setPosition(new_pos);
	
	// std::cout << "NEW HERO POS: " << new_pos.x << " " << new_pos.y << " " << std::endl;
}

void Graphic::next_hero_animation_frame()
{
	typedef std::pair<sf::Sprite, sf::Texture> sprite_texture_pair;
	typedef std::vector<sprite_texture_pair> sprite_texture_vector_of_pairs;

	// Directional textures.
	sprite_texture_vector_of_pairs& animation_frames = hero_sprites_with_texture.at(hero_looking_direction);

	if (animation_frames.size() <= ++hero_animation_frame)
		hero_animation_frame = 0;

	update_hero_texture_position();

	// std::cout << "UPDATED ANIMATION FRAME TO: " << int(hero_animation_frame) << std::endl;

}

void Graphic::move_hero(const Direction& direction)
{
	if (is_hero_moving)
		return;

	time_point_of_last_change_animation_frame = time_point_of_last_move_hero
		= std::chrono::steady_clock::now();

	hero_animation_frame = 0;
	move_step = 0;
	distance_left_to_move = CONSTS::TILE_SIZE;
	hero_looking_direction = direction;
	is_hero_moving = true;
}

void Graphic::draw_map()
{
	this->window->setView(map_view);
	window->draw(map.sprite);
}

void Graphic::update_view()
{
	// Margin for float imprecision included.
	float distance_to_left_wall = -1;
	float distance_to_right_wall = -1;
	float distance_to_top_wall = -1;
	float distance_to_bottom_wall = -1;

	distance_to_left_wall =
		(hero->position.x * CONSTS::TILE_SIZE + 3
			- (this->map_view.getCenter().x - this->map_view.getSize().x / 2))
		/ CONSTS::TILE_SIZE;

	distance_to_right_wall =
		(-(hero->position.x * CONSTS::TILE_SIZE - 3
			- (this->map_view.getCenter().x + this->map_view.getSize().x / 2))
			/ CONSTS::TILE_SIZE) - 1; // Recompense for hero size.

	distance_to_top_wall =
		(hero->position.y * CONSTS::TILE_SIZE + 3
			- (this->map_view.getCenter().y - this->map_view.getSize().y / 2))
		/ CONSTS::TILE_SIZE;

	distance_to_bottom_wall =
		(-(hero->position.y * CONSTS::TILE_SIZE - 3
			- (this->map_view.getCenter().y + this->map_view.getSize().y / 2))
			/ CONSTS::TILE_SIZE) - 1; // Recompense for hero size.

	if (distance_to_left_wall < CONSTS::MIN_PLAYER_DISTANCE_TO_BORDER)
		move_view(Direction::RIGHT);
	if (distance_to_right_wall < CONSTS::MIN_PLAYER_DISTANCE_TO_BORDER)
		move_view(Direction::LEFT);
	if (distance_to_top_wall < CONSTS::MIN_PLAYER_DISTANCE_TO_BORDER)
		move_view(Direction::BOTTOM);
	if (distance_to_bottom_wall < CONSTS::MIN_PLAYER_DISTANCE_TO_BORDER)
		move_view(Direction::TOP);

	// std::cout << distance_to_left_wall << " " << distance_to_right_wall << " " << distance_to_top_wall << " " << distance_to_bottom_wall << " " << std::endl;
	// std::cout << hero->position.x << " " << hero->position.y << std::endl;
}

void Graphic::update_hero()
{
	if (is_hero_moving)
	{
		update_hero_animation_frame();
		update_hero_step();
		
		update_view();
	}
}

sf::Vector2f Graphic::position_to_display_position(Position& position, sf::Sprite& entity_sprite)
{
	return sf::Vector2f(
		float(position.x * CONSTS::TILE_SIZE),
		float(position.y * CONSTS::TILE_SIZE - (uint16_t(entity_sprite.getGlobalBounds().height) % CONSTS::TILE_SIZE)));
}
