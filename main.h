#pragma once

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

constexpr static float TEXTURE_SIZE = 20.f;
constexpr static int CELL_SIZE_BORDER = 25;
constexpr static int CELL_SIZE = 24;
constexpr static float CELL_SIZEF_BORDER = static_cast<float>(CELL_SIZE_BORDER);
constexpr static float CELL_SIZEF = static_cast<float>(CELL_SIZE);

enum CELL_FLAGS {
    NEAR_BOMB = 0x0F,
    BOMB = 0x10,
    FLAG = 0x20,
    HIDDEN = 0x40,
};

struct Game {
    Game(int n_row, int n_col, int num_bombs, const char* title, bool vsync);

    ~Game();

    int row;
    int col;
    int num_bombs;
    bool ganhou;
    bool game_over;
    sf::RenderWindow window;
    sf::Texture texture;
    int8_t** grid;
    std::vector<sf::Vertex> vertices;
    std::vector<std::array<int, 2>> queue;

    void get_texture(sf::Vertex* v, int8_t cell) const;

    void render();

    bool verifica_ganhou() const;

    void draw_grid();

    void set_positions();

    void generate_grid();

    void open_cells(int startRow, int startCol);

    void display_bombs();

    void handle_mouse_release(const sf::Event& ev);

    void handle_keyboard(const sf::Event& ev);

    static void make_quad(sf::Vertex* v, sf::Color color, sf::Vector2f start, sf::Vector2f end);
};
