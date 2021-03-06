#include "Headers/grid.h"
#include "Headers/a-star-functions.h"
#include "Headers/prims.h"

int main()
{
	Grid               map{ MAP_WIDTH, MAP_HEIGHT };
	sf::RenderWindow   window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Path Finding");
	sf::Vector2i       mousePosition;
	std::vector<Cell*> path{};

	bool               diag = true;
	bool               pathUpdate = true;
	bool               pathBlocked = false; // Cannot route
	std::vector<Cell*> enclosedCellRoom{};
	sf::Clock          clock;

	int searchDrawingTime = 0;
	int pathDrawingTime = 0;
	while (window.isOpen())
	{
		int time = clock.restart().asMilliseconds();
		if (searchDrawingTime / SEARCHED_DISPLAY_RATE < static_cast<int>(map.getSearched().size()))
		{
			searchDrawingTime += time;
		}
		else pathDrawingTime += time;

		float xRatio = static_cast<float>(window.getSize().x) / WINDOW_WIDTH;
		float yRatio = static_cast<float>(window.getSize().y) / WINDOW_HEIGHT;
		mousePosition = sf::Mouse::getPosition(window);

		mousePosition.x -= static_cast<int>(static_cast<float>(window.getSize().x) / 2 - MAP_WIDTH /
			2.f * DEFAULT_TILE_SIZE * xRatio - DEFAULT_TILE_SIZE * xRatio);
		mousePosition.y -= static_cast<int>(static_cast<float>(window.getSize().y) / 2 - MAP_HEIGHT
			/ 2.f * DEFAULT_TILE_SIZE * yRatio - DEFAULT_TILE_SIZE * xRatio);

		sf::Vector2i mouseGridPos{
			mousePosition.x / static_cast<int>( (DEFAULT_TILE_SIZE * xRatio)),
			mousePosition.y / static_cast<int>( (DEFAULT_TILE_SIZE * yRatio))
		};

		// PROCESS EVENTS/ INPUTS
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyPressed)
			{
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
				{
					diag = !diag;
					pathUpdate = true;
					pathBlocked = false;
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::C))
				{
					map.resetGrid();
					enclosedCellRoom.clear();
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::V))
				{
					map.resetGrid();
					map.draw(window);
					Prims prims(MAP_WIDTH + 2, MAP_HEIGHT + 2);
					auto  maze = prims.generate();
					map.read(maze);
					map.draw(window, maze, sf::Color::White,
					         sf::RectangleShape(sf::Vector2f(DEFAULT_TILE_SIZE, DEFAULT_TILE_SIZE)), 0);
				}
			}
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
		{
			window.close();
		}

		// Updating and editting positions
		if (
			(sf::Keyboard::isKeyPressed(sf::Keyboard::A)
				|| sf::Keyboard::isKeyPressed(sf::Keyboard::B)
				|| sf::Mouse::isButtonPressed(sf::Mouse::Left)
				|| sf::Mouse::isButtonPressed(sf::Mouse::Right))
			&& map.inBounds(mouseGridPos.x, MAP_WIDTH + 1, mouseGridPos.y, MAP_HEIGHT + 1)
		)
		{
			if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
			{
				if (char c = map.at(mouseGridPos).c; c != ' ')
				{
					if (c == 'A')
					{
						map.resetA();
					}
					else if (c == 'B')
					{
						map.resetB();
					}

					map.at(mouseGridPos).makeEmpty();
					pathBlocked = false;
					pathUpdate = true;
				}
			}

			if (!map.at(mouseGridPos).solid)
			{
				if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
				{
					if (char c = map.at(mouseGridPos).c; c != ' ')
					{
						if (c == 'A')
						{
							map.resetA();
						}
						else if (c == 'B')
						{
							map.resetB();
						}
					}
					map.at(mouseGridPos).makeSolid();
					pathUpdate = true;
					
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
				{
					pathUpdate = map.moveA(viToVf(mouseGridPos));
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::B))
				{
					pathUpdate = map.moveB(viToVf(mouseGridPos));
				}
				if (pathBlocked && map.canRoute())
				{
					Cell* A = &map.at(map.getA());
					Cell* B = &map.at(map.getB());
					bool  containsA = contatinsPtr(enclosedCellRoom, A);
					bool  containsB = contatinsPtr(enclosedCellRoom, B);
					pathBlocked = !(containsA == containsB);
				}
			}
		}

		// UPDATE SCENE
		if (map.canRoute() && pathUpdate && !pathBlocked)
		{
			searchDrawingTime = 0;
			pathDrawingTime = 0;
			enclosedCellRoom = {};

			// Run A* and get path
			path = aStar(map, map.getA(), map.getB(), diag);

			// If no path can be made, it is blocked.
			pathBlocked = path.empty();

			// Get info about what's in the smallest room (whats being blocked)
			if (pathBlocked)
			{
				auto [_enclosedCell, _target, _enclosedCellRoom] = indiscriminateSearch(
					map, map.getA(), 'B', map.getB(), 'A', diag);

				for (auto var : _enclosedCellRoom)
				{
					enclosedCellRoom.push_back(var);
				}
			}
			pathUpdate = false;
		}

		if (!map.canRoute() || pathBlocked)
		{
			path = {};
			map.resetSearched();
		}
		// CLEAR SCENE
		window.clear();

		// DRAWING
		map.draw(window);

		map.draw(window,
		         enclosedCellRoom,
		         200, 200, 200, 100,
		         sf::CircleShape(DEFAULT_TILE_SIZE / 2.f),
		         static_cast<int>(enclosedCellRoom.size()));

		map.draw(window,
		         map.getSearched(),
		         220, 10, 230, 100,
		         sf::RectangleShape({ DEFAULT_TILE_SIZE, DEFAULT_TILE_SIZE }),
		         searchDrawingTime / SEARCHED_DISPLAY_RATE,
		         0.86f);

		map.draw(window,
		         path,
		         sf::Color::Yellow,
		         sf::RectangleShape({ DEFAULT_TILE_SIZE, DEFAULT_TILE_SIZE }),
		         pathDrawingTime / PATH_CONSTRUCTION_RATE,
		         0.42f);

		// DISPLAY
		window.display();
	}
}
