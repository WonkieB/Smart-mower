#include <iostream>
#include <string>
#include <algorithm>
#include <iomanip>
#include <vector>


#define ROWS 4
#define COLS 4
#define DONE 100


using namespace std;


enum class Actions {
    Up,
    Down,
    Left,
    Right
};

vector<vector<int> > paths(ROWS, vector<int>(COLS, 0));
long long int optimalSteps = INT_MAX;
uint16_t field = ROWS * COLS;
int currRew = 0;

class Mow {
public:
    struct Result {
        int reward;
        bool done;
        bool mowed;
    };

    Mow():
            m_board{
                    "SFFH",
                    "FHFH",
                    "HFFH",
                    "HFFF"
        }
            , m_X(0), m_Y(0) {

    }

private:
    char m_board[ROWS][COLS+ 1];
    int m_X, m_Y;

public:
    Result step(Actions action, long long int stepNum) {
        if (action == Actions::Up) {
            return Move(0, -1, stepNum);
        } else if (action == Actions::Down) {
            return Move(0, +1, stepNum);
        } else if (action == Actions::Left) {
            return Move(-1, 0, stepNum);
        } else if (action == Actions::Right) {
            return Move(+1, 0, stepNum);
        }

        return {0, true};
    }
    static bool checkField(){
        size_t sum = 0;
        for (auto &row:paths) {
            for (auto &col:row) {
                if (col) { sum += 1; }
            }
        }

        if (sum >= 0.5 * field) {
            return true;
        } else
            return false;
    }
    static void doneField(int agentX, int agentY) {
        paths[agentY][agentX] = 1;
    }

    void reset() {
        m_X = 0;
        m_Y = 0;
        for (auto &i : paths)
            std::fill(i.begin(), i.end(), 0);
    }

    void print() const {
        for (int y = 0; y < ROWS; ++y) {
            for (int x = 0; x < COLS; ++x) {
                if (m_X == x && m_Y == y) {
                    cout.put('@');
                } else {
                    cout.put(m_board[y][x]);
                }
            }

            cout << endl;
        }
    }

    int getX() const {
        return m_X;
    }

    int getY() const {
        return m_Y;
    }

private:
    Result Move(int x, int y, long long int step) {
        Result result;
        result.reward = step;
        result.done = false;
        result.mowed = false;


        m_X += x;
        m_Y += y;

        if (m_X < 0) {
            m_X = 0;
            result.reward = -1;
        } else if (m_X >= ROWS) {
            m_X = ROWS - 1;
            result.reward = -1;
        }

        if (m_Y < 0) {
            m_Y = 0;
            result.reward = -1;
        } else if (m_Y >= COLS) {
            m_Y = COLS - 1;
            result.reward = -1;
        }

        if (m_board[m_Y][m_X] == 'H') {
            result.reward = -1;
            result.done = true;
        }

        if (result.reward != -1){
            doneField(m_X, m_Y);
        }

        if (checkField()){
            result.done = true;
            result.mowed = true;
        }

        return result;
    }
};


class QLearner {

    // m_scoreMap[y][x][dir(up, down, left, right)] == (x, y)
    int m_scoreMap[ROWS][COLS][4];
    int m_finalMap[ROWS][COLS][4];

public:
    QLearner()
            : m_scoreMap{0}, m_finalMap{0} {

    }

    void cleanMap() {
        fill(*m_scoreMap[0], *m_scoreMap[COLS], 0);
    }
    bool homeChecker(){
        if (paths[0][0] == 1){
            for (int i = 0; i < 4; i++) {
                if(m_scoreMap[0][0][i] == 1){return true;}
            }
        }
        else {return false;}
    }

    void learn(Mow &mower, int totalEpochs){
        const Actions actionList[4] = {
                Actions::Up,
                Actions::Down,
                Actions::Left,
                Actions::Right
        };

        for (int epoch = 1; epoch <= totalEpochs; ++epoch) {
            mower.reset();
            cleanMap();

            for (long long int step = 0; step <= 100000; ++step) { // numeric_limits<long long int>::max()
                int actionIndex = rand() % 4;

                int oldX = mower.getX();
                int oldY = mower.getY();

                auto result = mower.step(actionList[actionIndex], step);

                int agentX = mower.getX();
                int agentY = mower.getY();

//              int maxQ = 0;//*std::max_element(m_scoreMap[agentY][agentX],m_scoreMap[agentY][agentX] + 4);
                m_scoreMap[oldY][oldX][actionIndex] = result.reward; //+ maxQ;

                if (result.mowed) {
                    m_scoreMap[agentY][agentX][actionIndex] = DONE;
                    if ((step < optimalSteps) && (homeChecker()))
                    {
                        //  przypisanie
                        optimalSteps = step;
                        for (int i = 0; i < ROWS; ++i) {
                            for (int j = 0; j < COLS; ++j) {
                                for (int k = 0; k < 4; ++k) {
                                    m_finalMap[i][j][k] = m_scoreMap[i][j][k];
                                }
                            }
                        }
                        // print
                       /* for (int i = 0; i < ROWS; i++) {
                            for (int j = 0; j < COLS; j++) {
                                cout << paths[i][j];
                            }
                            cout << endl;
                        }
                        cout << endl;*/
                    }
                    break;
                } else if (result.done) {
                    break;
                }
            }
        }
    }

    void play(Mow &game) const {
        const std::string actionName[] = {
                "Up", "Down", "Left", "Right"
        };

        game.reset();
        Mow::Result result;


        for (int step = 1; step <= 100; ++step) {
            int Xpos = game.getX();
            int Ypos = game.getY();
            int actionIndex = -1;
            int temp = 0;
            // find next step
            while (temp != currRew + 1) {
                actionIndex++;
                temp = m_finalMap[Ypos][Xpos][actionIndex];
                // if found DONE score
                if (step > 1 && temp == DONE) {
                    break;
                }
                // in case learning issue
                if (actionIndex > 3){
                    currRew++;
                    actionIndex = -1;
                }
            }
            currRew = currRew + 1;
            result = game.step(static_cast<Actions>(actionIndex), step);

            cout << "Action " << actionName[actionIndex] << " Score " << m_finalMap[Ypos][Xpos][actionIndex] << endl;
            game.print();

            cout << "----" << endl;
            if (result.mowed) {
                cout << "I did it!! Time to go home " << endl;
                break;
            }
        }
    }

    void print() const {
        for (int y = 0; y < ROWS; ++y) {
            for (int i = 0; i < 3; ++i) {
                for (int x = 0; x < COLS; ++x) {
                    if (i == 0) {
                        cout << "    ";
                        cout << setfill(' ') << setw(4) << m_finalMap[y][x][0];
                        cout << "    |";
                    } else if (i == 1) {
                        cout << setfill(' ') << setw(4) << m_finalMap[y][x][2];
                        cout << "    ";
                        cout << setfill(' ') << setw(4) << m_finalMap[y][x][3];
                        cout << "|";
                    } else {
                        cout << "    ";
                        cout << setfill(' ') << setw(4) << m_finalMap[y][x][1];
                        cout << "    |";
                    }
                }

                cout << endl;
            }

            cout << std::string((4 * 3 + 1) * COLS, '-') << endl;
        }
    }
};