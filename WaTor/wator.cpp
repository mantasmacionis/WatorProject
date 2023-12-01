#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>
#include <omp.h>
#include <vector>

const int NumShark = 20; 
const int NumFish = 3; 
const int FishBreed = 5; 
const int SharkBreed = 6; 
const int Starve = 4; 
const int GridSize = 155;
const int Threads = 1; 

int xdim = GridSize;
int ydim = GridSize;
int WindowXSize = 800; 
int WindowYSize = 600; 
int cellXSize = WindowXSize / xdim;
int cellYSize = WindowYSize / ydim;

enum CellType { EMPTY, FISH, SHARK };

struct Cell {
    CellType type;
    int breedTime;
    int starveTime;
    int age;
};

std::vector<std::vector<Cell>> watorGrid(xdim, std::vector<Cell>(ydim));



void moveFish(int i, int k) {
    int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    int randDir = std::rand() % 4;

    for (int d = 0; d < 4; ++d) {
        int newI = i + directions[(randDir + d) % 4][0];
        int newK = k + directions[(randDir + d) % 4][1];

        if (newI >= 0 && newI < xdim && newK >= 0 && newK < ydim && watorGrid[newI][newK].type == EMPTY) {
            std::swap(watorGrid[i][k], watorGrid[newI][newK]);
            return;
            
        }

    }
}

void moveShark(int i, int k) {
    int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    int randDir = std::rand() % 4;

    for (int d = 0; d < 4; ++d) {
        int newI = i + directions[(randDir + d) % 4][0];
        int newK = k + directions[(randDir + d) % 4][1];

        if (newI >= 0 && newI < xdim && newK >= 0 && newK < ydim) {
            if (watorGrid[newI][newK].type == FISH) {
                // Shark eats fish
                watorGrid[newI][newK].type = EMPTY;
                watorGrid[i][k].breedTime = 0; 
                watorGrid[i][k].starveTime = Starve; 
                std::swap(watorGrid[i][k], watorGrid[newI][newK]);
                return;
            }
        }
    }

    // If no fish found, move to an empty cell
    for (int d = 0; d < 4; ++d) {
        int newI = i + directions[(randDir + d) % 4][0];
        int newK = k + directions[(randDir + d) % 4][1];

        if (newI >= 0 && newI < xdim && newK >= 0 && newK < ydim && watorGrid[newI][newK].type == EMPTY) {
            std::swap(watorGrid[i][k], watorGrid[newI][newK]);
            watorGrid[i][k].starveTime--; 
            if (watorGrid[i][k].starveTime == 0) {
                watorGrid[i][k].type = EMPTY; 
            }
            return;
        }
    }
}

void initializeGrid() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    #pragma omp parallel for num_threads(Threads)
    for (int i = 0; i < xdim; ++i) {
        for (int k = 0; k < ydim; ++k) {
            watorGrid[i][k].type = EMPTY;
            watorGrid[i][k].breedTime = 0;
            watorGrid[i][k].starveTime = 0;
        }
    }

    // Place initial sharks
    for (int i = 0; i < NumShark; ++i) {
        int randX, randY;
        do {
            randX = std::rand() % xdim;
            randY = std::rand() % ydim;
        } while (watorGrid[randX][randY].type != EMPTY);

        watorGrid[randX][randY].type = SHARK;
    }

    // Place initial fish
    for (int i = 0; i < NumFish; ++i) {
        int randX, randY;
        do {
            randX = std::rand() % xdim;
            randY = std::rand() % ydim;
        } while (watorGrid[randX][randY].type != EMPTY);

        watorGrid[randX][randY].type = FISH;
    }
}

void updateGrid() {
    #pragma omp parallel for num_threads(Threads)
    for (int i = 0; i < xdim; ++i) {
        for (int k = 0; k < ydim; ++k) {
            if (watorGrid[i][k].type == FISH) {
                // Fish logic
                moveFish(i, k);
                watorGrid[i][k].breedTime++;
                if (watorGrid[i][k].breedTime >= FishBreed) {
                    int randI, randK;
                    do {
                        randI = std::rand() % xdim;
                        randK = std::rand() % ydim;
                    } while (watorGrid[randI][randK].type != EMPTY);

                    watorGrid[randI][randK].type = FISH;
                    watorGrid[randI][randK].breedTime = 0; // Reset breeding time
                }
            } else if (watorGrid[i][k].type == SHARK) {
                // Shark logic
                moveShark(i, k);
                watorGrid[i][k].breedTime++;
                if (watorGrid[i][k].breedTime >= SharkBreed) {
                    int randI, randK;
                    do {
                        randI = std::rand() % xdim;
                        randK = std::rand() % ydim;
                    } while (watorGrid[randI][randK].type != EMPTY);

                    watorGrid[randI][randK].type = SHARK;
                    watorGrid[randI][randK].breedTime = 0; // Reset breeding time
                }
            }
        }
    }
}

void drawGrid(sf::RenderWindow& window, sf::RectangleShape (&recArray)[GridSize][GridSize]) {
    #pragma omp parallel for num_threads(Threads)
    for (int i = 0; i < xdim; ++i) {
        for (int k = 0; k < ydim; ++k) {
            switch (watorGrid[i][k].type) {
                case EMPTY:
                    recArray[i][k].setFillColor(sf::Color::Blue);
                    break;
                case FISH:
                    recArray[i][k].setFillColor(sf::Color::Green);
                    break;
                case SHARK:
                    recArray[i][k].setFillColor(sf::Color::Red);
                    break;
            }
        }
    }

    // Draw the grid
    for (int i = 0; i < xdim; ++i) {
        for (int k = 0; k < ydim; ++k) {
            window.draw(recArray[i][k]);
        }
    }
}


int main() {


    double itime, ftime, exec_time;
    itime = omp_get_wtime();

    sf::RectangleShape recArray[GridSize][GridSize];

    for (int i = 0; i < xdim; ++i) {
        for (int k = 0; k < ydim; ++k) {
            recArray[i][k].setSize(sf::Vector2f(0.8f, 0.6f));  
            recArray[i][k].setPosition(i * cellXSize, k * cellYSize);
        }
    }

    sf::RenderWindow window(sf::VideoMode(WindowXSize, WindowYSize), "SFML Wa-Tor world");

    initializeGrid();

    int generations = 70; // 50 works
    for (int gen = 0; gen < generations; ++gen) {
        updateGrid();

        window.clear(sf::Color::Black);
        drawGrid(window, recArray);
        window.display();
    }

    ftime = omp_get_wtime();
    exec_time = ftime - itime;
    printf("\n\nTime taken is %f seconds\n", exec_time);

    return 0;
}