#include <iostream>
#include <string>
#include <algorithm>
#include <iomanip>
#include <vector>
//#include "mower.hpp"


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

class FrozenLake {
public:
    struct Result {
        int reward;
        bool done;
        bool mowed;
    };

    FrozenLake():
            m_board{"SFFF","FHFH","FFFH","HFFF"}

            , m_agentX(0), m_agentY(0) {

    }

private:
    char m_board[ROWS][COLS+ 1];
    int m_agentX, m_agentY;

public:
    Result step(Actions action, long long int stepNum) {
        if (action == Actions::Up) {
            return moveAgent(0, -1, stepNum);
        } else if (action == Actions::Down) {
            return moveAgent(0, +1, stepNum);
        } else if (action == Actions::Left) {
            return moveAgent(-1, 0, stepNum);
        } else if (action == Actions::Right) {
            return moveAgent(+1, 0, stepNum);
        }

        return {0, true};
    }

    static bool doneField(int agentX, int agentY) {
        paths[agentY][agentX] = 1;
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

    void reset() {
        m_agentX = 0;
        m_agentY = 0;
        for (auto &i : paths)
            std::fill(i.begin(), i.end(), 0);
    }

    void print() const {
        for (int y = 0; y < ROWS; ++y) {
            for (int x = 0; x < COLS; ++x) {
                if (m_agentX == x && m_agentY == y) {
                    cout.put('@');
                } else {
                    cout.put(m_board[y][x]);
                }
            }

            cout << endl;
        }
    }

    int getAgentX() const {
        return m_agentX;
    }

    int getAgentY() const {
        return m_agentY;
    }

private:
    Result moveAgent(int x, int y, long long int step) {
        Result result;
        result.reward = step;
        result.done = false;
        result.mowed = false;


        m_agentX += x;
        m_agentY += y;

        if (m_agentX < 0) {
            m_agentX = 0;
            result.reward = -1;
        } else if (m_agentX >= ROWS) {
            m_agentX = ROWS - 1;
            result.reward = -1;
        }


        if (m_agentY < 0) {
            m_agentY = 0;
            result.reward = -1;
        } else if (m_agentY >= COLS) {
            m_agentY = COLS - 1;
            result.reward = -1;
        }


        if (m_board[m_agentY][m_agentX] == 'H') {
            result.reward = -1;
            result.done = true;
        }
            /*else if (m_board[m_agentY][m_agentX] == 'G')
            {
                result.reward = 10;
                result.done = true;
            }*/


        else if (doneField(m_agentX, m_agentY)) {
            //result.reward = DONE;
            result.done = true;
            result.mowed = true;
        }

        return result;
    }
};


class QLearner {

    // m_scoreMap[y][x][dir(up, down, left, right)] == (x, y)에서 dir로 이동했을 때의 점수
    int m_scoreMap[ROWS][COLS][4];
    int m_finalMap[ROWS][COLS][4];

public:
    QLearner()
            : m_scoreMap{0}, m_finalMap{0} {

    }

    void cleanMap() {
        fill(*m_scoreMap[0], *m_scoreMap[COLS], 0);
    }

    void learn(FrozenLake &game, int totalEpochs){//, Lawn my_lawn) {
        const Actions actionList[4] = {
                Actions::Up,
                Actions::Down,
                Actions::Left,
                Actions::Right
        };

        for (int epoch = 1; epoch <= totalEpochs; ++epoch) {
            game.reset();
            cleanMap();

            for (long long int step = 0; step <= 10000; ++step) { // numeric_limits<long long int>::max()
                int actionIndex = rand() % 4;

                int oldX = game.getAgentX();
                int oldY = game.getAgentY();

                auto result = game.step(actionList[actionIndex], step);

                int agentX = game.getAgentX();
                int agentY = game.getAgentY();

//                int maxQ = 0;//*std::max_element(m_scoreMap[agentY][agentX],m_scoreMap[agentY][agentX] + 4);
//
                m_scoreMap[oldY][oldX][actionIndex] = result.reward; //+ maxQ;

                if (result.mowed) {
                    m_scoreMap[game.getAgentY()][game.getAgentX()][actionIndex] = DONE;
                    if ((step < optimalSteps) && (paths[0][0] == 1))
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
                        for (int i = 0; i < ROWS; i++) {
                            for (int j = 0; j < COLS; j++) {
                                cout << paths[i][j];
                            }
                            cout << endl;
                        }
                        cout << endl;
                    }
                    break;
                } else if (result.done) {
                    break;
                }
            }
        }
    }

    void play(FrozenLake &game) const {
        const std::string actionName[] = {
                "Up", "Down", "Left", "Right"
        };

        game.reset();
        FrozenLake::Result result;


        for (int step = 1; step <= 100; ++step) {
            int agentX = game.getAgentX();
            int agentY = game.getAgentY();
            int actionIndex = -1;
            int temp = 0;
            // find next step
            while (temp != currRew + 1) {
                actionIndex++;
                temp = m_finalMap[agentY][agentX][actionIndex];
                // if found DONE score
                if (temp == DONE) {
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

            cout << "Action " << actionName[actionIndex] << " Score " << m_finalMap[agentY][agentX][actionIndex] << endl;
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

/*
 *    FrozenLake game;
    QLearner agent;
    agent.learn(game, 1000);//, my_lawn);
    agent.print();
    agent.play(game);
 */