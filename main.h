#pragma once

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

constexpr int GRID_COL = 20;
constexpr int GRID_ROW = 20;
constexpr int NUM_BOMBS = (GRID_COL * GRID_ROW) * 0.10;
constexpr int CELL_SIZE_BORDER = 20;
constexpr int CELL_SIZE = 19;
constexpr float CELL_SIZEF_BORDER = static_cast<float>(CELL_SIZE_BORDER);
constexpr float CELL_SIZEF = static_cast<float>(CELL_SIZE);
constexpr int window_h = GRID_ROW * CELL_SIZE_BORDER;
constexpr int window_w = GRID_COL * CELL_SIZE_BORDER;

using Grid = std::array<std::array<int8_t, GRID_COL>, GRID_ROW>;

struct Game {
    Grid grid;
    sf::RenderWindow window;
    sf::Texture texture;
    sf::Sprite sprite;
    bool game_over;
    bool ganhou;

    Game(int w, int h, const char* title);

    void draw_grid();

    void get_texture(sf::Vertex* v, int8_t cell);

    void render();
};

bool verifica_ganhou(const Grid& grid);

void draw_grid(Game& game);

void generate_grid(Grid& grid);

void print_values(const Grid& grid);

void open_cells(Grid& grid, int startRow, int startCol);

void display_bombs(Grid& grid);

void handle_mouse_release(const sf::Event& ev, Grid& grid);

void handle_keyboard(const sf::Event& ev, sf::Window& window, Grid& grid);
