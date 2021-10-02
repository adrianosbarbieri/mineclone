#include <iostream>
#include <random>
#include <chrono>
#include <array>
#include <algorithm>
#include <queue>
#include "bit_helper.h"
#include "main.h"
#include <new>

void Game::get_texture(sf::Vertex* v, int8_t cell) const
{
    const static sf::Color COLOR_CELL = sf::Color::White;
    const static sf::Color COLOR_ZERO = { 77, 77, 77, 255 };
    const static sf::Color COLOR_GREEN = sf::Color::Green;
    const static sf::Color COLOR_RED = sf::Color::Red;

    if (has(cell, FLAG)) {
        float texture_offset = TEXTURE_SIZE * 1.f;
        v[0].color = COLOR_CELL;
        v[1].color = COLOR_CELL;
        v[2].color = COLOR_CELL;
        v[3].color = COLOR_CELL;
        v[0].texCoords = { texture_offset, 0.f };
        v[1].texCoords = { texture_offset, TEXTURE_SIZE };
        v[2].texCoords = { texture_offset + TEXTURE_SIZE, TEXTURE_SIZE };
        v[3].texCoords = { texture_offset + TEXTURE_SIZE, 0.f };
    } else if (has(cell, HIDDEN)) {
        v[0].color = COLOR_CELL;
        v[1].color = COLOR_CELL;
        v[2].color = COLOR_CELL;
        v[3].color = COLOR_CELL;
        v[0].texCoords = { 0.f, 0.f };
        v[1].texCoords = { 0.f, TEXTURE_SIZE };
        v[2].texCoords = { TEXTURE_SIZE, TEXTURE_SIZE };
        v[3].texCoords = { TEXTURE_SIZE, 0.f };
    } else if (has(cell, BOMB) && !ganhou) {
        v[0].color = COLOR_RED;
        v[1].color = COLOR_RED;
        v[2].color = COLOR_RED;
        v[3].color = COLOR_RED;
        v[0].texCoords = { 0.f, 0.f };
        v[1].texCoords = { 0.f, TEXTURE_SIZE };
        v[2].texCoords = { TEXTURE_SIZE, TEXTURE_SIZE };
        v[3].texCoords = { TEXTURE_SIZE, 0.f };
    } else if (has(cell, BOMB) && ganhou) {
        v[0].color = COLOR_GREEN;
        v[1].color = COLOR_GREEN;
        v[2].color = COLOR_GREEN;
        v[3].color = COLOR_GREEN;
        v[0].texCoords = { 0.f, 0.f };
        v[1].texCoords = { 0.f, TEXTURE_SIZE };
        v[2].texCoords = { TEXTURE_SIZE, TEXTURE_SIZE };
        v[3].texCoords = { TEXTURE_SIZE, 0.f };
    } else if (!has(cell, NEAR_BOMB)) {
        v[0].color = COLOR_ZERO;
        v[1].color = COLOR_ZERO;
        v[2].color = COLOR_ZERO;
        v[3].color = COLOR_ZERO;
        v[0].texCoords = { 0.f, 0.f };
        v[1].texCoords = { 0.f, TEXTURE_SIZE };
        v[2].texCoords = { TEXTURE_SIZE, TEXTURE_SIZE };
        v[3].texCoords = { TEXTURE_SIZE, 0.f };
    } else {
        float texture_offset = TEXTURE_SIZE * (has(cell, NEAR_BOMB) + 1);
        v[0].color = COLOR_CELL;
        v[1].color = COLOR_CELL;
        v[2].color = COLOR_CELL;
        v[3].color = COLOR_CELL;
        v[0].texCoords = { texture_offset, 0.f };
        v[1].texCoords = { texture_offset, TEXTURE_SIZE };
        v[2].texCoords = { texture_offset + TEXTURE_SIZE, TEXTURE_SIZE };
        v[3].texCoords = { texture_offset + TEXTURE_SIZE, 0.f };
    }
}

bool Game::verifica_ganhou() const
{
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
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
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            int8_t val = grid[i][j];
            sf::Vertex* vertex = &vertices[(i + j * row) * 4];
            get_texture(vertex, val);
        }
    }

    sf::RenderStates state;
    state.texture = &texture;
    window.draw(&vertices[0], vertices.size(), sf::Quads, state);
}

void Game::set_positions()
{
    for (int i = 0; i < row; i++) {
        float posi = CELL_SIZEF_BORDER * i;

        for (int j = 0; j < col; j++) {
            float posj = CELL_SIZEF_BORDER * j;

            sf::Vertex* vertex = &vertices[(i + j * row) * 4];

            vertex[0].position = { 0.f + posj, 0.f + posi };
            vertex[1].position = { 0.f + posj, CELL_SIZEF + posi };
            vertex[2].position = { CELL_SIZEF + posj, CELL_SIZEF + posi };
            vertex[3].position = { CELL_SIZEF + posj, 0.f + posi };
        }
    }
}

void Game::render()
{
    window.clear();
    draw_grid();
    window.display();
}

void Game::generate_grid()
{
    static std::random_device randomDevice;
    static std::default_random_engine engine(randomDevice());
    static std::vector<std::array<int8_t, 2>> indexes(row * col);

    std::fill_n(&grid[0][0], row * col, static_cast<int8_t>(HIDDEN));

    size_t n = 0;

    for (int8_t i = 0; i < row; i++) {
        for (int8_t j = 0; j < col; j++) {
            indexes[n++] = { i, j };
        }
    }

    std::shuffle(indexes.begin(), indexes.end(), engine);

    for (size_t i = 0; i < num_bombs; i++) {
        set(grid[indexes[i][0]][indexes[i][1]], BOMB);
    }

    constexpr int nextRows[] = { -1, -1, -1, 0, 0, 1, 1, 1 };
    constexpr int nextCols[] = { -1, 0, 1, -1, 1, -1, 0, 1 };

    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            if (has(grid[i][j], BOMB)) {
                for (int k = 0; k < 8; k++) {
                    int row_i = i + nextRows[k];
                    int col_i = j + nextCols[k];

                    if (row_i >= 0 && row_i < row && col_i >= 0 && col_i < col) {
                        if (!has(grid[row_i][col_i], BOMB)) {
                            grid[row_i][col_i]++;
                        }
                    }
                }
            }
        }
    }
}

void Game::open_cells(int startRow, int startCol)
{
    constexpr int nextRows[] = { -1, -1, -1, 0, 0, 0, 1, 1, 1 };
    constexpr int nextCols[] = { -1, 0, 1, -1, 0, 1, -1, 0, 1 };

    const int size = row * col;
    static std::vector<std::array<int, 2>> q(size);

    int qfront = 0;
    int qback = 0;
    int qcount = 0;

    q[qback++] = { startRow, startCol };
    qcount++;

    while (qcount > 0) {
        std::array<int, 2> idx = q[qfront++];
        qfront = qfront % size;
        qcount--;

        int8_t& val = grid[idx[0]][idx[1]];

        if (has(val, HIDDEN)) {
            clear(val, HIDDEN);

            if (!has(val, BOMB) && !has(val, NEAR_BOMB)) {
                for (int i = 0; i < 9; i++) {
                    int row_i = idx[0] + nextRows[i];
                    int col_i = idx[1] + nextCols[i];

                    if (row_i >= 0 && row_i < row && col_i >= 0 && col_i < col) {
                        q[qback++] = { row_i, col_i };
                        qback = qback % size;
                        qcount++;
                    }
                }
            }
        }
    }
}

void Game::display_bombs()
{
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            if (has(grid[i][j], BOMB)) {
                clear(grid[i][j], FLAG);
                clear(grid[i][j], HIDDEN);
            }
        }
    }
}

void Game::handle_mouse_release(const sf::Event& ev)
{
    if (game_over) {
        return;
    }

    const int posX = ev.mouseButton.x;
    const int posY = ev.mouseButton.y;
    const int cellX = posX / CELL_SIZE_BORDER;
    const int cellY = posY / CELL_SIZE_BORDER;
    const int remX = posX % CELL_SIZE_BORDER;
    const int remY = posY % CELL_SIZE_BORDER;

    if (remX < CELL_SIZE && remY < CELL_SIZE) {
        int8_t& val = grid[cellY][cellX];

        if (ev.mouseButton.button == sf::Mouse::Button::Right) {
            if (has(val, HIDDEN)) {
                toggle(val, FLAG);
            }
        } else if (ev.mouseButton.button == sf::Mouse::Button::Left) {
            if (!has(val, FLAG)) {
                if (has(val, BOMB)) {
                    game_over = true;
                    display_bombs();
                    return;
                } else if (has(val, HIDDEN)) {
                    open_cells(cellY, cellX);
                }
            }
        }
    }

    if (verifica_ganhou()) {
        ganhou = true;
        game_over = true;
        display_bombs();
    }
}

void Game::handle_keyboard(const sf::Event& ev)
{
    switch (ev.key.code) {
    case sf::Keyboard::Enter:
        game_over = false;
        ganhou = false;
        generate_grid();
        break;

    case sf::Keyboard::Escape:
        if (!game_over) {
            game_over = true;
            ganhou = false;
            display_bombs();
        }
        break;
    }
}

Game::Game(int n_row, int n_col, int num_bombs, const char* title, bool vsync)
    : row(n_row)
    , col(n_col)
    , num_bombs(num_bombs)
    , ganhou(false)
    , game_over(false)
    , window(sf::VideoMode(n_row * CELL_SIZE_BORDER, n_col * CELL_SIZE_BORDER), title, sf::Style::Close)
    , vertices(4 * n_row * n_col)
{
    window.setVerticalSyncEnabled(vsync);
    int8_t* a = new int8_t[row * col];
    grid = new int8_t*[row];

    grid[0] = &a[0];

    for (int i = 1; i < row; i++) {
        grid[i] = grid[0] + col * i;
    }
}

Game::~Game()
{
    delete[] grid[0];
    delete[] grid;
}

float get_average_frametime(const std::array<int64_t, 32>& a)
{
    return std::accumulate(a.begin(), a.end(), 0) / a.size();
}

int main(int argc, const char* argv[])
{
    using namespace std::chrono;

    int col;
    int row;
    int num_bombs;

    switch (argc) {
    case 4:
        col = atoi(argv[2]);
        row = atoi(argv[1]);
        num_bombs = atoi(argv[3]);
        break;

    case 3:
        col = atoi(argv[2]);
        row = atoi(argv[1]);
        num_bombs = col * row * 0.1;
        break;

    default:
        col = 16;
        row = 16;
        num_bombs = col * row * 0.1;
        break;
    }

    if (num_bombs > col * row) {
        num_bombs = col * row * 0.1;
    }

    Game game(row, col, num_bombs, "Mines", true);
    game.texture.loadFromFile("Icons.png");
    game.texture.setSmooth(true);
    game.set_positions();
    game.generate_grid();

    steady_clock::time_point t1 = steady_clock::now();

    int64_t acum = 0;

    constexpr int64_t second = 1'000'000'000;
    constexpr float dsecond = 1'000'000'000.f;

    char fpsTitle[32] = {};
    std::array<int64_t, 32> frametimes = {};

    int frametimes_i = 0;

    while (game.window.isOpen()) {
        steady_clock::time_point t2 = steady_clock::now();
        int64_t diff = (t2 - t1).count();
        acum += diff;

        frametimes[frametimes_i] = diff;
        frametimes_i++;
        frametimes_i = frametimes_i % 32;

        if (acum > second) {
            float fps = dsecond / get_average_frametime(frametimes);
            snprintf(fpsTitle, 32, "Mineclone %f", fps);
            game.window.setTitle(fpsTitle);
            acum -= second;
        }

        t1 = t2;

        sf::Event ev;

        while (game.window.pollEvent(ev)) {
            switch (ev.type) {
            case sf::Event::Closed:
                game.window.close();
                break;

            case sf::Event::MouseButtonReleased:
                game.handle_mouse_release(ev);
                break;

            case sf::Event::KeyPressed:
                game.handle_keyboard(ev);
                break;
            }
        }

        game.render();
    }

    return 0;
}
