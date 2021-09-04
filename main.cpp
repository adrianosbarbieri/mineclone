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

void Game::draw_grid()
{
    sf::RectangleShape rect(sf::Vector2f(CELL_SIZEF, CELL_SIZEF));

    const static sf::Color COLOR_HIDDEN = { 127, 127, 127 };
    const static sf::Color COLOR_ZERO = { 77, 77, 77 };
    const static sf::Color COLOR_BLUE = { 20, 20, 99 };
    const sf::Color COLOR_BOMB = ganhou ? sf::Color(20, 255, 20) : sf::Color(255, 20, 20);

    for (int i = 0; i < GRID_ROW; i++) {
        for (int j = 0; j < GRID_COL; j++) {
            const float fi = static_cast<float>(i);
            const float fj = static_cast<float>(j);

            int8_t val = grid[i][j];

            rect.setPosition({ fj * CELL_SIZEF_BORDER, fi * CELL_SIZEF_BORDER });

            if (has(val, FLAG)) {
                rect.setFillColor(COLOR_HIDDEN);
                sprite.setPosition({ fj * CELL_SIZEF_BORDER, fi * CELL_SIZEF_BORDER });
                sprite.setTextureRect(sf::IntRect(0, 0, CELL_SIZE, CELL_SIZE));
                window.draw(rect);
                window.draw(sprite);
            } else if (has(val, HIDDEN)) {
                rect.setFillColor(COLOR_HIDDEN);
                window.draw(rect);
            } else if (has(val, BOMB)) {
                rect.setFillColor(COLOR_BOMB);
                window.draw(rect);
            } else if (!(has(val, NEAR_BOMB))) {
                rect.setFillColor(COLOR_ZERO);
                window.draw(rect);
            } else {
                rect.setFillColor(COLOR_BLUE);
                sprite.setPosition({ fj * CELL_SIZEF_BORDER, fi * CELL_SIZEF_BORDER });
                sprite.setTextureRect({ CELL_SIZE_BORDER * has(val, NEAR_BOMB), 0, CELL_SIZE, CELL_SIZE });
                window.draw(rect);
                window.draw(sprite);
            }
        }
    }
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
                    display_bombs(g.grid);
                    g.game_over = true;
                    return;
                } else if (has(val, HIDDEN)) {
                    open_cells(g.grid, cellY, cellX);
                }
            }
        }
    }

    if (verifica_ganhou(g.grid)) {
        display_bombs(g.grid);
        g.ganhou = true;
        g.game_over = true;
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
    //game.window.setVerticalSyncEnabled(true);
    game.texture.loadFromFile("Icons20.png");
    game.sprite.setTexture(game.texture);

    using namespace std::chrono;

    steady_clock::time_point t1 = steady_clock::now();
    steady_clock::time_point t2;

    int64_t acum = 0;

    while (game.window.isOpen()) {
        t2 = steady_clock::now();

        int64_t diff = (t2 - t1).count();

        acum += diff;

        if (acum > 1'000'000) {
            // 1s
            game.window.setTitle(std::to_string(1000000000.0 / diff));
            acum -= 1'000'000;
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
