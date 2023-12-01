/**
 * @file wator.cpp
 * @brief main method and WaTor implementation.
 * @author Mantas Macionis
 * @date 01-12-2023
 * @see https://github.com/mantasmacionis/ccDevLabs/
 * @license Creative Commons Attribution-NonCommercial-ShareAlike 4.0
 */
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

/**
 * @brief Enumeration representing the possible cell types in the simulation grid.
 */
enum CellType { EMPTY, FISH, SHARK };

/**
 * @brief Structure representing a cell in the Wator simulation grid.
 */
struct Cell {
    CellType type;
    int breedTime;
    int starveTime;
    int age;
};

/**
 * @brief 2D vector representing the Wator simulation grid.
 *
 * The grid contains cells of type Cell, forming a 2D matrix.
 * @param xdim The width (number of columns) of the grid.
 * @param ydim The height (number of rows) of the grid.
 */
std::vector<std::vector<Cell>> watorGrid(xdim, std::vector<Cell>(ydim));


/**
 * @brief Move a fish to an adjacent empty cell.
 *
 * This function moves a fish from the specified position (i, k) to an adjacent
 * empty cell in the Wator simulation grid. The fish randomly selects one of the four directions
 * (up, down, left, right) to move.
 *
 * @param i The row index of the current position.
 * @param k The column index of the current position.
 */
void moveFish(int i, int k) {
    int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    int randDir = std::rand() % 4;

     // Iterate over the four directions in a random order
    for (int d = 0; d < 4; ++d) {
        int newI = i + directions[(randDir + d) % 4][0];
        int newK = k + directions[(randDir + d) % 4][1];

        // Check if the new position is within the grid boundaries and the cell is empty
        if (newI >= 0 && newI < xdim && newK >= 0 && newK < ydim && watorGrid[newI][newK].type == EMPTY) {
            std::swap(watorGrid[i][k], watorGrid[newI][newK]);
            return;
            
        }

    }
}

/**
 * @brief Move a shark to an adjacent cell and potentially eat a fish.
 *
 * This function moves a shark from the specified position (i, k) to an adjacent cell
 * in the Wator simulation grid. The shark randomly selects one of the four directions (up, down, left, right) to move.
 * If the target cell contains a fish, the shark eats the fish and updates its status.
 *
 * @param i The row index of the current position.
 * @param k The column index of the current position.
 */
void moveShark(int i, int k) {
    // Define possible directions: up, down, left, right
    int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    int randDir = std::rand() % 4;

    // Iterate over the four directions in a random order
    for (int d = 0; d < 4; ++d) {
        int newI = i + directions[(randDir + d) % 4][0];
        int newK = k + directions[(randDir + d) % 4][1];

         // Check if the new position is within the grid boundaries
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

   /**
 * @brief Move a shark to an adjacent empty cell if no fish is found.
 *
 * This part of the code is executed when a shark cannot find a fish in any of the adjacent cells.
 * It tries to move the shark to an empty cell in one of the four directions (up, down, left, right).
 * If successful, the shark's starvation time is decremented, and if it reaches zero, the shark dies.
 *
 * @param i The row index of the current position.
 * @param k The column index of the current position.
 */
    for (int d = 0; d < 4; ++d) {
        // Calculate the new position based on the current direction
        int newI = i + directions[(randDir + d) % 4][0];
        int newK = k + directions[(randDir + d) % 4][1];

         // Check if the new position is within the grid boundaries and the cell is empty
        if (newI >= 0 && newI < xdim && newK >= 0 && newK < ydim && watorGrid[newI][newK].type == EMPTY) {
            std::swap(watorGrid[i][k], watorGrid[newI][newK]);
            // Decrement the shark's starvation time
            watorGrid[i][k].starveTime--; 
             // Check if the shark has starved to death
            if (watorGrid[i][k].starveTime == 0) {
                watorGrid[i][k].type = EMPTY; 
            }
            return;
        }
    }
}

/**
 * @brief Initialize the Wator simulation grid with empty cells and place initial organisms.
 *
 * This function initializes the Wator simulation grid by setting all cells to be empty.
 * It then randomly places the initial population of sharks and fish on the grid.
 * The number of threads used for parallel execution is controlled by the `Threads` constant.
 */
void initializeGrid() {
    // Seed the random number generator based on the current time
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    #pragma omp parallel for num_threads(Threads)
    for (int i = 0; i < xdim; ++i) {
        for (int k = 0; k < ydim; ++k) {
             // Set cell properties to default values
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
        // Set the cell type to SHARK
        watorGrid[randX][randY].type = SHARK;
    }

    // Place initial fish
    for (int i = 0; i < NumFish; ++i) {
        int randX, randY;
        do {
            // Find a random empty cell for the fish
            randX = std::rand() % xdim;
            randY = std::rand() % ydim;
        } while (watorGrid[randX][randY].type != EMPTY);
         // Set the cell type to FISH
        watorGrid[randX][randY].type = FISH;
    }
}

/**
 * @brief Update the Wator simulation grid based on the rules for fish and sharks.
 *
 * This function iterates through each cell in the Wator simulation grid and updates the state
 * of fish and sharks according to their specific rules. It includes movement, breeding, and age-related logic.
 */
void updateGrid() {
    #pragma omp parallel for num_threads(Threads)
    for (int i = 0; i < xdim; ++i) {
        for (int k = 0; k < ydim; ++k) {
             // Check the type of organism in the current cell
            if (watorGrid[i][k].type == FISH) {
                // Fish logic
                moveFish(i, k);
                watorGrid[i][k].breedTime++;
                // Check if the fish can breed
                if (watorGrid[i][k].breedTime >= FishBreed) {
                    int randI, randK;
                    // Find a random empty cell for the new fish
                    do {
                        randI = std::rand() % xdim;
                        randK = std::rand() % ydim;
                    } while (watorGrid[randI][randK].type != EMPTY);
                    // Place a new fish in the empty cell
                    watorGrid[randI][randK].type = FISH;
                    watorGrid[randI][randK].breedTime = 0; // Reset breeding time
                }
            } else if (watorGrid[i][k].type == SHARK) {
                // Shark logic
                // Move the shark and potentially eat a fish
                moveShark(i, k);
                watorGrid[i][k].breedTime++;
                 // Check if the shark can breed
                if (watorGrid[i][k].breedTime >= SharkBreed) {
                    int randI, randK;
                    do {
                        randI = std::rand() % xdim;
                        randK = std::rand() % ydim;
                    } while (watorGrid[randI][randK].type != EMPTY);
                    // Place a new shark in the empty cell
                    watorGrid[randI][randK].type = SHARK;
                    watorGrid[randI][randK].breedTime = 0; // Reset breeding time
                }
            }
        }
    }
}

/**
 * @brief Draw the Wator simulation grid on the SFML window using rectangles.
 *
 * This function updates the color of rectangles in the array based on the type of organism
 * (EMPTY, FISH, SHARK) in each corresponding cell of the Wator simulation grid. The colored rectangles
 * are then drawn on the SFML window to represent the state of the simulation.
 *
 * @param window The SFML window on which the grid will be drawn.
 * @param recArray A 2D array of SFML RectangleShapes representing the grid cells.
 */
void drawGrid(sf::RenderWindow& window, sf::RectangleShape (&recArray)[GridSize][GridSize]) {
    #pragma omp parallel for num_threads(Threads)
    for (int i = 0; i < xdim; ++i) {
        for (int k = 0; k < ydim; ++k) {
            // Set rectangle color based on the type of organism in the grid cell
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

/**
 * @brief Main function for the Wa-Tor simulation.
 *
 * This function initializes the SFML window, grid, and executes the Wa-Tor simulation for a specified
 * number of generations. The execution time of the simulation is printed at the end.
 *
 * @return 0 upon successful completion.
 */
int main() {


    double itime, ftime, exec_time;
     // Record the start time
    itime = omp_get_wtime();

     // Create an array of SFML RectangleShapes representing the grid cells
    sf::RectangleShape recArray[GridSize][GridSize];
     // Initialize the size and position of each rectangle in the array
    for (int i = 0; i < xdim; ++i) {
        for (int k = 0; k < ydim; ++k) {
            recArray[i][k].setSize(sf::Vector2f(0.8f, 0.6f));  
            recArray[i][k].setPosition(i * cellXSize, k * cellYSize);
        }
    }
    //SFML Window creation
    sf::RenderWindow window(sf::VideoMode(WindowXSize, WindowYSize), "SFML Wa-Tor world");

    initializeGrid();
    //Number of generations to continue simulation for.
    int generations = 70; 
    for (int gen = 0; gen < generations; ++gen) {
        updateGrid();

        window.clear(sf::Color::Black);
        drawGrid(window, recArray);
        window.display();
    }
    // Record the end time and calculate the execution time
    ftime = omp_get_wtime();
    exec_time = ftime - itime;
    //Print time (Recorded for speedup report)
    printf("\n\nTime taken is %f seconds\n", exec_time);

    return 0;
}