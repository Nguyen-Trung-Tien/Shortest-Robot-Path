#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <algorithm>
#include <string>

using namespace std;

const int CELL_SIZE = 130; // Kích thước mỗi ô trong mê cung
const sf::Color WALL_COLOR = sf::Color(20, 20, 20); // Màu tường
const sf::Color PATH_COLOR = sf::Color(20, 20, 20); // Màu đường đi
const sf::Color ROBOT_COLOR1 = sf::Color::Yellow;     // Màu robot 1
const sf::Color ROBOT_COLOR2 = sf::Color::Green;    // Màu robot 2

struct Maze {
    vector<vector<int>> cells;
    int rows;
    int cols;
};


struct Robot {
    int startX;
    int startY;
    int score;
    set<pair<int, int>> visited;
    set<pair<int, int>> commonVisited;
    string name;
};



Maze readMazeFromFile(const string& filename) {
    ifstream file(filename);
    Maze maze;
    if (file.is_open()) {
        file >> maze.rows >> maze.cols;
        maze.cells.resize(maze.rows, vector<int>(maze.cols));
        for (int i = 0; i < maze.rows; ++i) {
            for (int j = 0; j < maze.cols; ++j) {
                file >> maze.cells[i][j];
            }
        }
        file.close();
    }
    return maze;
}



void findPath(const Maze& maze, Robot& robot, sf::RenderWindow& window, sf::Sound& sound, const sf::Color& robotColor) {
    int x = robot.startX;
    int y = robot.startY;
    // Khởi tạo font cho sf::Text
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) { 
        std::cout << "Khong the tai font Arial!" << std::endl;
        return;
    }

    // Tạo một đối tượng sf::Text để hiển thị số
    sf::Text numberText;
    numberText.setFont(font);
    numberText.setCharacterSize(50); // Đổi kích thước chữ số thành 40
    numberText.setFillColor(sf::Color::Red);


    while (true) {
        robot.visited.insert({ x, y });
        robot.score += maze.cells[x][y];

        if (maze.cells[x][y] == 9) {
            break;
        }

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return;
            }
        }

        window.clear();

        // Vẽ mê cung
        for (int i = 0; i < maze.rows; ++i) {
            for (int j = 0; j < maze.cols; ++j) {
                sf::RectangleShape cell(sf::Vector2f(CELL_SIZE, CELL_SIZE));
                cell.setPosition(j * CELL_SIZE, i * CELL_SIZE);
                if (maze.cells[i][j] == 1) {
                    cell.setFillColor(WALL_COLOR);
                }
                else {
                    cell.setFillColor(PATH_COLOR);
                }
                window.draw(cell);
            }
        }

        // Vẽ đường đi của robot
        for (const auto& cell : robot.visited) {
            sf::RectangleShape visitedCell(sf::Vector2f(CELL_SIZE, CELL_SIZE));
            visitedCell.setPosition(cell.second * CELL_SIZE, cell.first * CELL_SIZE);
            visitedCell.setFillColor(robotColor);
            window.draw(visitedCell);
        }

        // Vẽ robot
        sf::RectangleShape robotShape(sf::Vector2f(CELL_SIZE - 2, CELL_SIZE - 2)); // Đổi kích thước ô để chúng không che phủ lẫn nhau
        robotShape.setPosition(y * CELL_SIZE + 1, x * CELL_SIZE + 1); // Đặt lại vị trí của ô
        robotShape.setFillColor(robotColor);
        window.draw(robotShape);

        // Vẽ số của ô mà robot đã đi qua bằng sf::Text
        numberText.setString(std::to_string(maze.cells[x][y])); // Chuyển giá trị thành chuỗi và đặt nội dung cho sf::Text
        numberText.setPosition(y * CELL_SIZE + CELL_SIZE / 2 - 12, x * CELL_SIZE + CELL_SIZE / 2 - 12); // Đặt vị trí của sf::Text
        window.draw(numberText);

        window.display();

        sound.play();
        sf::sleep(sf::milliseconds(1000));


        vector<pair<int, int>> possibleMoves;

        // Tìm ô có số lớn nhất và các ô đi được (chỉ trong bốn hướng cơ bản: lên, xuống, trái, phải)

        int maxScore = 0;

        // Kiểm tra ô phía trên

        int newX = x - 1;
        int newY = y;
        if (newX >= 0 && maze.cells[newX][newY] != 1 && robot.visited.find({ newX, newY }) == robot.visited.end()) {
            possibleMoves.push_back({ newX, newY });
            maxScore = maze.cells[newX][newY];
        }

        // Kiểm tra ô phía dưới

        newX = x + 1;
        if (newX < maze.rows && maze.cells[newX][newY] != 1 && robot.visited.find({ newX, newY }) == robot.visited.end()) {

            if (maze.cells[newX][newY] > maxScore) {

                possibleMoves.clear();
                maxScore = maze.cells[newX][newY];
            }

            if (maze.cells[newX][newY] == maxScore) {
                possibleMoves.push_back({ newX, newY });
            }
        }

        // Kiểm tra ô bên trái

        newX = x;
        newY = y - 1;
        if (newY >= 0 && maze.cells[newX][newY] != 1 && robot.visited.find({ newX, newY }) == robot.visited.end()) {
            if (maze.cells[newX][newY] > maxScore) {
                possibleMoves.clear();
                maxScore = maze.cells[newX][newY];
            }
            if (maze.cells[newX][newY] == maxScore) {
                possibleMoves.push_back({ newX, newY });
            }
        }

        // Kiểm tra ô bên phải

        newY = y + 1;
        if (newY < maze.cols && maze.cells[newX][newY] != 1 && robot.visited.find({ newX, newY }) == robot.visited.end()) {
            if (maze.cells[newX][newY] > maxScore) {
                possibleMoves.clear();
                maxScore = maze.cells[newX][newY];
            }
            if (maze.cells[newX][newY] == maxScore) {
                possibleMoves.push_back({ newX, newY });
            }
        }

        // Chọn ngẫu nhiên một ô trong các ô đi được có giá trị lớn nhất

        if (!possibleMoves.empty()) {
            random_shuffle(possibleMoves.begin(), possibleMoves.end());
            x = possibleMoves[0].first;
            y = possibleMoves[0].second;
        }
        else {
            break;
        }
    }
}



void printInstructions() {
    cout << "***************************************************************************************************************" << endl;
    cout << "*                                 Chao mung den voi tro choi duong di robot                                   *" << endl;
    cout << "* Hai robot se cung di qua mot me cung va di thu thap diem so tren cac o trong me cung.                       *" << endl;
    cout << "* Nhiem vu cua ban la di chuyen cac robot den diem sao cho tong diem so la cao nhat.                          *" << endl;
    cout << "*                                                                                                             *" << endl;
    cout << "* Huong dan choi:                                                                                             *" << endl;
    cout << "* - Ban nhap toa do (x y) cho hai robot (Robot 1 va Robot 2) tai diem bat dau cua chung trong me cung.        *" << endl;
    cout << "* - Hai robot se di chuyen tren cac o trong me cung.                                                          *" << endl;
    cout << "* - Diem so cua robot tang len khi di qua cac o trong me cung.                                                *" << endl;
    cout << "* - Cac o trong me cung co gia tri 1 la tuong, cac o co gia tri khac 1 la duong di.                           *" << endl;
    cout << "* - Robot se dung lai khi dat duoc diem den                                                                   *" << endl;
    cout << "* - Robot se di chuyen ngau nhien trong cac o khong bi tuong va chua duoc robot di qua truoc do.              *" << endl;
    cout << "* - Khi ket thuc tro choi, ket qua se duoc luu vao file output.txt.                                           *" << endl;
    cout << "***************************************************************************************************************" << endl;
    cout << endl;
}



void showAuthorInfo() {
    cout << "***************************************************************************************************************" << endl;
    cout << "*                                 * Ten: Nguyen Trung Tien *                                                  *" << endl;
    cout << "*                                 * Lop: CN22H             *                                                  *" << endl;
    cout << "*                                 * MSSV: 2251120447       *                                                  *" << endl;
    cout << "***************************************************************************************************************" << endl;
    cout << endl;
}

void displayMenu() {
    cout << "********************** MENU **********************" << endl;
    cout << "1. Robot 1                                       *" << endl;
    cout << "2. Robot 2                                       *" << endl;
    cout << "3. Ca 2 robot cung di                            *" << endl;
    cout << "4. Thoat                                         *" << endl;
    cout << "**************************************************" << endl;
    cout << "Nhap lua chon cua ban: ";
}



void enterRobotCoordinates(Robot& robot, const string& robotName) {
    cout << "Nhap (x y) diem bat dau " << robotName << " (vd: 0 0): ";
    cin >> robot.startX >> robot.startY;
    robot.score = 0;
    robot.name = robotName;
}



void saveResultToFile(const Maze& maze, const Robot& robot1, const Robot& robot2, const vector<pair<int, int>>& commonPath) {

    ofstream file("output.txt");

    if (file.is_open()) {
        file << "Ten: Nguyen Trung Tien" << endl;
        file << "Lop: CN22H" << endl;
        file << "MSSV: 2251120447" << endl;

        file << "Cac buoc di cua " << robot1.name << ": ";
        for (const auto& cellValue : robot1.visited) {
            file << maze.cells[cellValue.first][cellValue.second] << " ";
        }

        file << endl;

        file << "Cac buoc di cua " << robot2.name << ": ";
        for (const auto& cellValue : robot2.visited) {
            file << maze.cells[cellValue.first][cellValue.second] << " ";
        }

        file << endl;

        file << "Vi tri di trung nhau cua hai robot: ";
        if (!commonPath.empty()) {
            for (const auto& cell : commonPath) {
                file << maze.cells[cell.first][cell.second] << " ";
            }
        }

        else {
            file << "Khong co vi tri di trung nhau.";
        }
        file << endl;

        file.close();
    }
    else {
        cout << "Không thể mở file để ghi kết quả!" << endl;
    }
}


void playGame(const int mode) {
    Maze maze = readMazeFromFile("input.txt");
    sf::RenderWindow window(sf::VideoMode(maze.cols * CELL_SIZE, maze.rows * CELL_SIZE), "Robot Maze Game");
    sf::SoundBuffer buffer;
    buffer.loadFromFile("beep.wav");
    sf::Sound sound(buffer);

    Robot robot1, robot2;

    if (mode == 1 || mode == 3) {
        enterRobotCoordinates(robot1, "Robot 1");
    }
    if (mode == 2 || mode == 3) {
        enterRobotCoordinates(robot2, "Robot 2");
    }

    vector<pair<int, int>> commonPath;

    if (mode == 1 || mode == 3) {
        findPath(maze, robot1, window, sound, ROBOT_COLOR1);
    }
    if (mode == 2 || mode == 3) {
        findPath(maze, robot2, window, sound, ROBOT_COLOR2);
    }

    if (mode == 3) {
        // Tìm các bước đi trùng nhau của 2 robot
        set_intersection(robot1.visited.begin(), robot1.visited.end(), robot2.visited.begin(), robot2.visited.end(), back_inserter(commonPath));
    }

    saveResultToFile(maze, robot1, robot2, commonPath);

    cout << "Da luu ket qua vao file output.txt" << endl;

    if (mode != 3) {
        Robot winner;
        if (mode == 1) {
            winner = robot1;
        }
        else {
            winner = robot2;
        }

        if (winner.score == 0) {
            std::cout << "Khong co robot nao dat duoc diem dich.\n";
        }
        else {
            std::cout << "Robot " << winner.name << " chien thang voi diem so la " << winner.score << " diem.\n";
        }
    }

    else {
        std::cout << "Ket qua:\n";
        std::cout << "Diem Robot 1: " << robot1.score << " diem.\n";
        std::cout << "Diem Robot 2: " << robot2.score << " diem.\n";

        if (robot1.score > robot2.score) {
            std::cout << "Robot 1 chien thang voi diem so cao nhat.\n";
        }
        else if (robot1.score < robot2.score) {
            std::cout << "Robot 2 chien thang voi diem so cao nhat.\n";
        }
        else {
            std::cout << "Hai robot co diem so bang nhau.\n";
        }
    }
}




int main() {
    int choice;

    printInstructions();
    showAuthorInfo();


    do {
        displayMenu();
        cin >> choice;

        switch (choice) {
        case 1:
            playGame(1);
            break;
        case 2:
            playGame(2);
            break;
        case 3:
            playGame(3);
            break;
        case 4:
            cout << "Thoát khỏi trò chơi.\n";
            break;
        default:
            cout << "Lựa chọn không hợp lệ. Vui lòng chọn lại.\n";
            break;
        }

        cout << endl;
    } while (choice != 4);

    return 0;
}
