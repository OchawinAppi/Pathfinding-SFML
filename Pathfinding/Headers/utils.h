#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>

inline sf::Vector2i vfToVi(const sf::Vector2f &vec)
{
	return { static_cast<int>(vec.x), static_cast<int>(vec.y) };
}

inline sf::Vector2f viToVf(const sf::Vector2i &vec)
{
	return { static_cast<float>(vec.x), static_cast<float>(vec.y) };
}

inline bool isAnyKeyPressed()
{
	for (int k = -1; k < sf::Keyboard::KeyCount; ++k)
	{
		if (sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(k)))
			return true;
	}
	return false;
}

template <typename  ... Args>
constexpr void logT(const std::string_view title, Args&& ...args)
{
	std::cout << "== " << title << " ==\n";
	([](const auto& x) { std::cout << ": " << x << " "; }(std::forward<Args>(args)), ...);
	std::cout << '\n';
}

template <typename ... Args>
constexpr void log(Args&& ... args)
{
	([](const auto& x) { std::cout << ": " << x << " "; }(std::forward<Args>(args)), ...);
	std::cout << '\n';
}

inline sf::Vector2f offset(const sf::Vector2f& pos)
{
	return {
		pos.x * DEFAULT_TILE_SIZE + static_cast<float>(MAP_WIDTH) / 2 - MAP_WIDTH / 2.f * DEFAULT_TILE_SIZE - DEFAULT_TILE_SIZE,
		pos.y * DEFAULT_TILE_SIZE + static_cast<float>(WINDOW_HEIGHT) / 2 - MAP_HEIGHT / 2.f * DEFAULT_TILE_SIZE - DEFAULT_TILE_SIZE
	};
}