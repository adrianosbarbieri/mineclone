#include <iostream>
#include <string>
#include <random>
#include <chrono>
#include <array>
#include <algorithm>
#include <queue>
#include <iomanip>
#include "bit_helper.h"
#include "main.h"

enum CELL_FLAGS {
    NEAR_BOMB = 0x0F,
    BOMB = 0x10,
    FLAG = 0x20,
    HIDDEN = 0x40,
};

Game::Game(int w, int h, const char* title)
    : grid {}
    , window(sf::VideoMode(w, h), title, sf::Style::Close)
    , game_over(false)
    , ganhou(false)
{
    generate_grid(grid);
}

void make_quad_pos(sf::Vertex* v, float ini, float end)
{
    v[0].position = { ini, ini };
    v[1].position = { ini, end };
    v[2].position = { end, end };
    v[3].position = { end, ini };
}

void Game::get_texture(sf::Vertex* v, int8_t cell)
{
    const static sf::Color COLOR_HIDDEN = { 127, 127, 127, 255 };
    const static sf::Color COLOR_ZERO = { 77, 77, 77, 255 };
    const static sf::Color COLOR_BLUE = { 30, 30, 120, 255 };
    const sf::Color COLOR_BOMB = ganhou ? sf::Color(20, 255, 20, 255) : sf::Color(255, 20, 20, 255);

    if (has(cell, FLAG)) {
        v[0].color = COLOR_HIDDEN;
        v[1].color = COLOR_HIDDEN;
        v[2].color = COLOR_HIDDEN;
        v[3].color = COLOR_HIDDEN;
        v[0].texCoords = { 0.f, 0.f };
        v[1].texCoords = { 0.f, CELL_SIZEF };
        v[2].texCoords = { CELL_SIZEF, CELL_SIZEF };
        v[3].texCoords = { CELL_SIZEF, 0.f };
    } else if (has(cell, HIDDEN)) {
        v[0].color = COLOR_HIDDEN;
        v[1].color = COLOR_HIDDEN;
        v[2].color = COLOR_HIDDEN;
        v[3].color = COLOR_HIDDEN;
    } else if (has(cell, BOMB)) {
        v[0].color = COLOR_BOMB;
        v[1].color = COLOR_BOMB;
        v[2].color = COLOR_BOMB;
        v[3].color = COLOR_BOMB;
    } else if (!has(cell, NEAR_BOMB)) {
        v[0].color = COLOR_ZERO;
        v[1].color = COLOR_ZERO;
        v[2].color = COLOR_ZERO;
        v[3].color = COLOR_ZERO;
    } else {
        v[0].color = COLOR_BLUE;
        v[1].color = COLOR_BLUE;
        v[2].color = COLOR_BLUE;
        v[3].color = COLOR_BLUE;

        float textureoffset = static_cast<float>(CELL_SIZEF_BORDER * has(cell, NEAR_BOMB));

        v[0].texCoords = { textureoffset, 0.f };
        v[1].texCoords = { textureoffset, CELL_SIZEF_BORDER };
        v[2].texCoords = { textureoffset + 19.f, CELL_SIZEF_BORDER };
        v[3].texCoords = { textureoffset + 19.f, 0.f };
    }
}

bool verifica_ganhou(const Grid& grid)
{
    for (int i = 0; i < GRID_ROW; i++) {
        for (int j = 0; j < GRID_COL; j++) {
            int8_t val = grid[i][j];

            if (has(val, HIDDEN) && has(val, NEAR_BOMB)) {
                return false;
            }

            if (has(val, BOMB) && !has(val, FLAG)) {
                return false;
            }
        }
    }

    return true;
}

std::array<sf::Vertex, 4 * GRID_ROW * GRID_COL> vertices;

void Game::draw_grid()
{
    vertices = {};

    for (int i = 0; i < GRID_ROW; i++) {
        float posi = CELL_SIZEF_BORDER * i;

        for (int j = 0; j < GRID_COL; j++) {
            float posj = CELL_SIZEF_BORDER * j;

            int8_t val = grid[i][j];

            sf::Vertex* vertex = &vertices[(j + i * GRID_ROW) * 4];

            vertex[0].position = { 0.f + posj, 0.f + posi };
            vertex[1].position = { 0.f + posj, 19.f + posi };
            vertex[2].position = { 19.f + posj, 19.f + posi };
            vertex[3].position = { 19.f + posj, 0.f + posi };

            get_texture(vertex, val);
        }
    }

    sf::RenderStates state;
    state.texture = &texture;

    window.draw(&vertices[0], vertices.size(), sf::Quads, state);
}

void Game::render()
{
    window.clear();
    draw_grid();
    window.display();
}

void generate_grid(Grid& grid)
{
    static std::random_device randomDevice;
    static std::default_random_engine engine(randomDevice());
    std::array<std::array<int8_t, 2>, GRID_COL * GRID_ROW> indexes;

    std::fill_n(grid.begin()->begin(), GRID_COL * GRID_ROW, HIDDEN);

    size_t n = 0;

    for (int8_t i = 0; i < GRID_ROW; i++) {
        for (int8_t j = 0; j < GRID_COL; j++) {
            indexes[n++] = { i, j };
        }
    }

    std::shuffle(indexes.begin(), indexes.end(), engine);

    for (size_t i = 0; i < NUM_BOMBS; i++) {
        set(grid[indexes[i][0]][indexes[i][1]], BOMB);
    }

    constexpr int nextRows[] = { -1, -1, -1, 0, 0, 1, 1, 1 };
    constexpr int nextCols[] = { -1, 0, 1, -1, 1, -1, 0, 1 };

    for (int i = 0; i < GRID_ROW; i++) {
        for (int j = 0; j < GRID_COL; j++) {
            if (has(grid[i][j], BOMB)) {
                for (int k = 0; k < 8; k++) {
                    int row = i + nextRows[k];
                    int col = j + nextCols[k];

                    if (row >= 0 && row < GRID_ROW && col >= 0 && col < GRID_COL) {
                        if (!has(grid[row][col], BOMB)) {
                            grid[row][col]++;
                        }
                    }
                }
            }
        }
    }
}

void print_values(const Grid& grid)
{
    for (int i = 0; i < GRID_ROW; i++) {
        for (int j = 0; j < GRID_COL; j++) {
            int8_t val = grid[i][j];
            std::cout << std::setw(2) << (int)val << ' ';
        }

        std::cout << '\n';
    }
}

void open_cells(Grid& grid, int startRow, int startCol)
{
    constexpr int nextRows[] = { -1, -1, -1, 0, 0, 0, 1, 1, 1 };
    constexpr int nextCols[] = { -1, 0, 1, -1, 0, 1, -1, 0, 1 };

    std::queue<std::array<int, 2>> q;
    q.push({ startRow, startCol });

    while (q.size() > 0) {
        std::array<int, 2> idx = q.front();
        q.pop();

        int8_t& val = grid[idx[0]][idx[1]];

        if (has(val, HIDDEN)) {
            clear(val, HIDDEN);

            if (!(val & BOMB) && !(val & 0x0F)) {
                for (int i = 0; i < 9; i++) {
                    int row = idx[0] + nextRows[i];
                    int col = idx[1] + nextCols[i];

                    if (row >= 0 && row < GRID_ROW
                        && col >= 0 && col < GRID_COL) {
                        q.push({ row, col });
                    }
                }
            }
        }
    }
}

void display_bombs(Grid& grid)
{
    for (int i = 0; i < GRID_ROW; i++) {
        for (int j = 0; j < GRID_COL; j++) {
            if (has(grid[i][j], BOMB)) {
                clear(grid[i][j], FLAG);
                clear(grid[i][j], HIDDEN);
            }
        }
    }
}

void handle_mouse_release(const sf::Event& ev, Game& g)
{
    if (g.game_over) {
        return;
    }

    const int posX = ev.mouseButton.x;
    const int posY = ev.mouseButton.y;

    const int cellX = posX / CELL_SIZE_BORDER;
    const int cellY = posY / CELL_SIZE_BORDER;
    const int remX = posX % CELL_SIZE_BORDER;
    const int remY = posY % CELL_SIZE_BORDER;

    if (remX < CELL_SIZE && remY < CELL_SIZE) {
        int8_t& val = g.grid[cellY][cellX];

        if (ev.mouseButton.button == sf::Mouse::Button::Right) {
            if (has(val, HIDDEN)) {
                toggle(val, FLAG);
            }
        } else if (ev.mouseButton.button == sf::Mouse::Button::Left) {
            if (!has(val, FLAG)) {
                if (has(val, BOMB)) {
                    g.game_over = true;
                    display_bombs(g.grid);
                    return;
                } else if (has(val, HIDDEN)) {
                    open_cells(g.grid, cellY, cellX);
                }
            }
        }
    }

    if (verifica_ganhou(g.grid)) {
        g.ganhou = true;
        g.game_over = true;
        display_bombs(g.grid);
    }
}

void handle_keyboard(const sf::Event& ev, Game& g)
{
    switch (ev.key.code) {
    case sf::Keyboard::Enter:
        g.game_over = false;
        g.ganhou = false;
        g.grid = {};
        generate_grid(g.grid);
        break;
    case sf::Keyboard::Escape:
        g.game_over = true;
        g.ganhou = false;
        display_bombs(g.grid);
    }
}

int main()
{
    Game game(window_w, window_h, "Mineclone");
    game.window.setVerticalSyncEnabled(true);
    game.texture.loadFromFile("Icons20.png");
    game.texture.setSmooth(true);
    game.sprite.setTexture(game.texture);

    using namespace std::chrono;

    steady_clock::time_point t1 = steady_clock::now();
    steady_clock::time_point t2;

    int64_t acum = 0;

    while (game.window.isOpen()) {

        steady_clock::time_point t2 = steady_clock::now();

        int64_t diff = (t2 - t1).count();

        acum += diff;

        if (acum > 1'000'000'000) {
            // 1s
            game.window.setTitle(std::to_string(1000000000.0 / diff));
            acum -= 1'000'000'000;
        }

        t1 = t2;

        sf::Event ev;

        while (game.window.pollEvent(ev)) {
            switch (ev.type) {
            case sf::Event::Closed:
                game.window.close();
                break;

            case sf::Event::MouseButtonReleased:
                handle_mouse_release(ev, game);
                break;

            case sf::Event::KeyPressed:
                handle_keyboard(ev, game);
                break;
            }
        }

        game.render();
    }

    return 0;
}
