#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <iomanip>

using namespace std;

const int BOARD_SIZE = 15;
const int CENTER = 7; // 天元 (8,8)

enum Piece { EMPTY = 0, BLACK = 1, WHITE = 2 };
enum GameMode { MODE_SPECIFIED = 1, MODE_FREE = 2 };

struct Point {
    int r, c;
    bool operator==(const Point& other) const { return r == other.r && c == other.c; }
};

// --- 棋盘类 ---
class Board {
private:
    int grid[BOARD_SIZE][BOARD_SIZE];

public:
    Board() { reset(); }

    void reset() {
        for (int i = 0; i < BOARD_SIZE; i++)
            for (int j = 0; j < BOARD_SIZE; j++)
                grid[i][j] = EMPTY;
    }

    bool place(int r, int c, Piece p) {
        if (r < 0 || r >= BOARD_SIZE || c < 0 || c >= BOARD_SIZE) return false;
        if (grid[r][c] != EMPTY) return false;
        grid[r][c] = p;
        return true;
    }

    bool isEmpty(int r, int c) const {
        if (r < 0 || r >= BOARD_SIZE || c < 0 || c >= BOARD_SIZE) return false;
        return grid[r][c] == EMPTY;
    }

    void draw(const vector<Point>& candidates = {}) const {
        cout << "   ";
        for (int i = 0; i < BOARD_SIZE; i++) cout << setw(2) << i + 1 << " ";
        cout << endl;
        for (int i = 0; i < BOARD_SIZE; i++) {
            cout << setw(2) << i + 1 << " ";
            for (int j = 0; j < BOARD_SIZE; j++) {
                bool isCand = false;
                for (size_t k = 0; k < candidates.size(); k++) {
                    if (candidates[k].r == i && candidates[k].c == j) {
                        cout << "[" << k + 1 << "]";
                        isCand = true; break;
                    }
                }
                if (isCand) continue;

                if (grid[i][j] == EMPTY) {
                    if (i == CENTER && j == CENTER) cout << " + ";
                    else cout << " . ";
                } else if (grid[i][j] == BLACK) cout << " X ";
                else cout << " O ";
            }
            cout << endl;
        }
    }

    // 检查胜利
    // strictRules = true: 执行禁手（黑长连不算赢）
    // strictRules = false: 自由规则（黑白长连都算赢）
    int checkWin(bool strictRules) {
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                if (grid[i][j] == EMPTY) continue;
                int color = grid[i][j];
                int dr[] = {0, 1, 1, 1};
                int dc[] = {1, 0, 1, -1};

                for (int d = 0; d < 4; d++) {
                    int count = 1;
                    // 向前检查
                    for (int step = 1; step < 6; step++) {
                        int nr = i + step * dr[d], nc = j + step * dc[d];
                        if (nr >= 0 && nr < BOARD_SIZE && nc >= 0 && nc < BOARD_SIZE && grid[nr][nc] == color) count++;
                        else break;
                    }

                    if (count == 5) {
                        // 检查长连 (Overline)
                        int nextR = i + 5 * dr[d], nextC = j + 5 * dc[d];
                        bool overline = false;
                        if (nextR >= 0 && nextR < BOARD_SIZE && nextC >= 0 && nextC < BOARD_SIZE && grid[nextR][nextC] == color)
                            overline = true;
                        
                        // 反向检查前一个，防止从中间开始算
                        int prevR = i - dr[d], prevC = j - dc[d];
                        if (prevR >= 0 && prevR < BOARD_SIZE && prevC >= 0 && prevC < BOARD_SIZE && grid[prevR][prevC] == color)
                            continue;

                        // 规则判断
                        if (!strictRules) return color; // 自由模式：只要5个及以上就赢
                        if (color == WHITE) return WHITE; // 白棋长连也赢
                        if (color == BLACK && !overline) return BLACK; // 严格模式：黑棋必须刚好5个
                        // 严格模式黑棋长连为禁手，这里简单处理为不赢
                    }
                }
            }
        }
        return 0;
    }
};

// --- 游戏控制类 ---
class RenjuGame {
private:
    Board board;
    int moveCount;
    int playerBlack; // 执黑玩家ID (1或2)
    int playerWhite; // 执白玩家ID
    GameMode currentMode;

    Point getInput(string prompt) {
        Point p;
        while (true) {
            cout << prompt << " (行 列): ";
            if (cin >> p.r >> p.c) {
                p.r--; p.c--;
                if (board.isEmpty(p.r, p.c)) return p;
                cout << "位置无效或已有棋子。\n";
            } else {
                cin.clear(); cin.ignore(1000, '\n');
                cout << "格式错误。\n";
            }
        }
        return p;
    }

public:
    RenjuGame() : moveCount(0), playerBlack(1), playerWhite(2) {}

    // 核心流程 1：指定开局逻辑 (复杂)
    void runSpecifiedOpening() {
        cout << "\n--- 进入 [指定开局] 流程 ---\n";
        cout << "* 规则：黑1天元，黑定白2，黑3限区，三手交换，五手N打，黑棋禁手。\n\n";

        // 1. 黑1 (天元)
        cout << "[第1手] 黑方必须下天元。\n";
        board.place(CENTER, CENTER, BLACK);
        moveCount++; 
        board.draw();

        // 2. 白2 (黑方指定)
        Point p2 = getInput("请执黑方指定 [白2] 位置");
        board.place(p2.r, p2.c, WHITE);
        moveCount++;
        board.draw();

        // 3. 黑3 (天元5x5范围)
        while(true) {
            Point p3 = getInput("请执黑方落 [黑3] (需在天元5x5范围内)");
            if (abs(p3.r - CENTER) <= 2 && abs(p3.c - CENTER) <= 2) {
                board.place(p3.r, p3.c, BLACK);
                moveCount++;
                break;
            }
            cout << "违规！黑3超出了5x5范围。\n";
        }
        board.draw();

        // 4. 三手交换 (核心要求 6)
        cout << "\n=== 三手交换环节 ===\n";
        cout << "轮到执白方 (玩家" << playerWhite << ") 决策：\n";
        cout << "输入 1: 交换黑白 (你将执黑)\n输入 0: 保持不变 (继续执白)\n选择: ";
        int swap;
        cin >> swap;
        if (swap == 1) {
            std::swap(playerBlack, playerWhite);
            cout << ">> 双方已交换颜色！\n";
        } else {
            cout << ">> 白方选择不交换。\n";
        }
        cout << "当前状态: 玩家" << playerBlack << " 执黑, 玩家" << playerWhite << " 执白。\n";

        // 5. 白4 (自由)
        Point p4 = getInput("请执白方落 [白4]");
        board.place(p4.r, p4.c, WHITE);
        moveCount++;
        board.draw();

        // 6. 五手N打
        cout << "\n=== 五手 N 打环节 ===\n";
        int N;
        while(true) {
            cout << "请执黑方指定打点数 N (2-5): ";
            cin >> N;
            if (N>=2 && N<=5) break;
        }
        
        vector<Point> candidates;
        cout << "请执黑方输入 " << N << " 个候选点：\n";
        for(int i=0; i<N; i++) {
            while(true) {
                cout << "候选点 " << i+1 << ": ";
                Point c; cin >> c.r >> c.c; c.r--; c.c--;
                if(board.isEmpty(c.r, c.c)) {
                    candidates.push_back(c); break;
                }
                cout << "该点被占用。\n";
            }
        }
        cout << "\n黑方提供的候选点如下：\n";
        board.draw(candidates);

        cout << "请执白方选择其中一个作为黑5 (输入序号 1-" << N << "): ";
        int idx;
        cin >> idx; // 简化处理，假设输入合法
        Point p5 = candidates[idx-1];
        board.place(p5.r, p5.c, BLACK);
        moveCount++;
        cout << ">> 白方选择了点 " << idx << "，定为黑5。\n";
        board.draw();
    }

    // 核心流程 2：自由开局逻辑 (简单)
    void runFreeOpening() {
        cout << "\n--- 进入 [自由开局] 流程 ---\n";
        cout << "* 规则：黑1、白2、黑3 自由落子，无交换，无N打，无禁手。\n\n";

        // 黑1
        Point p1 = getInput("[第1手] 请执黑方自由落子");
        board.place(p1.r, p1.c, BLACK);
        moveCount++;
        board.draw();

        // 白2
        Point p2 = getInput("[第2手] 请执白方自由落子");
        board.place(p2.r, p2.c, WHITE);
        moveCount++;
        board.draw();

        // 黑3
        Point p3 = getInput("[第3手] 请执黑方自由落子");
        board.place(p3.r, p3.c, BLACK);
        moveCount++;
        board.draw();
        
        cout << ">> 自由开局结束，进入正常对局。\n";
    }

    // 主程序入口
    void start() {
        cout << "================================\n";
        cout << "    五子棋对战系统 V2.0\n";
        cout << "================================\n";
        cout << "请选择开局模式：\n";
        cout << "1. 指定开局 (专业规则：交换、N打、禁手)\n";
        cout << "2. 自由开局 (娱乐规则：自由落子、无禁手)\n";
        cout << "选择: ";
        int mode;
        cin >> mode;
        currentMode = (mode == 1) ? MODE_SPECIFIED : MODE_FREE;

        board.draw();

        // 执行对应的开局逻辑
        if (currentMode == MODE_SPECIFIED) {
            runSpecifiedOpening();
        } else {
            runFreeOpening();
        }

        // 进入通用对局循环
        gameLoop();
    }

    void gameLoop() {
        cout << "\n=== 正常对局阶段 ===\n";
        while (true) {
            // 判定胜负 (根据模式决定是否启用严格禁手规则)
            bool strict = (currentMode == MODE_SPECIFIED);
            int winner = board.checkWin(strict);
            
            if (winner != 0) {
                cout << "\n#################################\n";
                cout << "   " << (winner == BLACK ? "黑方 (Player " : "白方 (Player ") 
                     << (winner == BLACK ? playerBlack : playerWhite) << ") 获胜！\n";
                cout << "#################################\n";
                break;
            }

            // 判断轮次：moveCount是偶数时轮到黑(因为数组下标0是第一手)，奇数轮到白
            // 修正：moveCount记录的是已经下的棋子数。
            // 5颗子已下 -> moveCount=5 -> 轮到白棋 (moveCount%2 != 0)
            Piece currentPiece = (moveCount % 2 == 0) ? BLACK : WHITE;
            int currentPlayerID = (currentPiece == BLACK) ? playerBlack : playerWhite;

            cout << "\n[第 " << moveCount + 1 << " 手] 轮到 " 
                 << (currentPiece == BLACK ? "黑方" : "白方") 
                 << " (Player " << currentPlayerID << ") 落子。\n";
            
            Point p = getInput("请输入落子坐标");
            board.place(p.r, p.c, currentPiece);
            moveCount++;
            board.draw();
        }
    }
};

int main() {
    RenjuGame game;
    game.start();
    return 0;
}