#include <stdio.h>
#include <stdlib.h>
#include <windows.h> // 用于Sleep()和system("cls")，如果是Linux/Mac需替换为unistd.h

// 定义迷宫的最大尺寸
#define MAX_HEIGHT 30
#define MAX_WIDTH 30

// 定义地图元素
#define WALL 1      // 墙壁
#define ROAD 0      // 通路
#define VISITED 2   // 已走过的路径
#define CURRENT 3   // 当前探索位置
#define BACKTRACK 4 // 回溯路径（死胡同）

// 迷宫结构体
typedef struct {
    int grid[MAX_HEIGHT][MAX_WIDTH];
    int rows;
    int cols;
    int startX, startY;
    int endX, endY;
} Maze;

Maze m; // 全局迷宫对象

// 清屏并打印迷宫（实现动态效果）
void printMaze() {
    // 移动光标到(0,0)而不是清屏，可以减少闪烁（Windows特定优化）
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = {0, 0};
    SetConsoleCursorPosition(hOut, pos);

    printf("######## 迷宫探险中 ########\n");
    for (int i = 0; i < m.rows; i++) {
        for (int j = 0; j < m.cols; j++) {
            if (i == m.startX && j == m.startY) printf("S "); // 起点
            else if (i == m.endX && j == m.endY) printf("E "); // 终点
            else if (m.grid[i][j] == WALL) printf("█ ");
            else if (m.grid[i][j] == ROAD) printf("  ");
            else if (m.grid[i][j] == VISITED) printf("· "); // 正确路径的一部分
            else if (m.grid[i][j] == CURRENT) printf("@ "); // 当前探头
            else if (m.grid[i][j] == BACKTRACK) printf("x "); // 走不通回退的路
        }
        printf("\n");
    }
    printf("############################\n");
    Sleep(200); // 暂停200毫秒，控制动画速度
}

// 深度优先搜索 (DFS)
// 返回 1 表示找到出口，0 表示未找到
int dfs(int x, int y) {
    // 1. 边界检查与合法性检查
    if (x < 0 || x >= m.rows || y < 0 || y >= m.cols) return 0;
    if (m.grid[x][y] == WALL || m.grid[x][y] == VISITED || m.grid[x][y] == BACKTRACK) return 0;

    // 2. 到达终点
    if (x == m.endX && y == m.endY) {
        return 1;
    }

    // 3. 标记当前点为“正在访问”
    m.grid[x][y] = CURRENT; 
    printMaze(); // 动态刷新显示

    // 将当前点标记为已访问（假设是路径的一部分）
    m.grid[x][y] = VISITED;

    // 4. 递归探索四个方向 (上, 右, 下, 左)
    // 顺序不同，找出的路径可能不同
    int dx[] = {-1, 0, 1, 0};
    int dy[] = {0, 1, 0, -1};

    for (int i = 0; i < 4; i++) {
        if (dfs(x + dx[i], y + dy[i])) {
            return 1; // 如果某一个方向找到了出口，这就是一条通路，直接返回
        }
    }

    // 5. 回溯：如果四个方向都走不通，说明由于当前点导致的死胡同
    m.grid[x][y] = BACKTRACK; // 标记为死路
    printMaze(); // 动态显示回退过程
    
    return 0;
}

// 初始化不同大小的迷宫
void initMaze(int level) {
    // 重置地图
    for(int i=0; i<MAX_HEIGHT; i++)
        for(int j=0; j<MAX_WIDTH; j++)
            m.grid[i][j] = ROAD;

    if (level == 1) { 
        // 5x5 小型迷宫
        m.rows = 5; m.cols = 5;
        m.startX = 0; m.startY = 0;
        m.endX = 4; m.endY = 4;
        // 构建围墙和障碍
        int map[5][5] = {
            {0, 1, 0, 0, 0},
            {0, 1, 0, 1, 0},
            {0, 0, 0, 1, 0},
            {0, 1, 1, 1, 0},
            {0, 0, 0, 1, 0}
        };
        for(int i=0;i<5;i++) for(int j=0;j<5;j++) m.grid[i][j] = map[i][j];
    } 
    else if (level == 2) {
        // 10x10 中型迷宫
        m.rows = 10; m.cols = 10;
        m.startX = 1; m.startY = 1;
        m.endX = 8; m.endY = 8;
        // 构建围墙（四周是墙，中间随机一点）
        for(int i=0; i<10; i++) { m.grid[0][i]=1; m.grid[9][i]=1; m.grid[i][0]=1; m.grid[i][9]=1; }
        // 手动加一些障碍
        m.grid[1][1]=0; m.grid[8][8]=0;
        m.grid[2][2]=1; m.grid[2][3]=1; m.grid[3][3]=1; m.grid[4][1]=1;
        m.grid[5][5]=1; m.grid[5][6]=1; m.grid[5][7]=1; m.grid[6][5]=1;
        m.grid[1][5]=1; m.grid[2][5]=1; m.grid[3][5]=1; 
    }
    else {
        // 15x20 大型迷宫
        m.rows = 15; m.cols = 20;
        m.startX = 1; m.startY = 1;
        m.endX = 13; m.endY = 18;
         // 简单生成全包围墙
        for(int i=0; i<m.rows; i++) for(int j=0; j<m.cols; j++) 
            if(i==0 || i==m.rows-1 || j==0 || j==m.cols-1) m.grid[i][j] = 1;
        
        // 模拟复杂的蛇形墙
        for(int j=4; j<16; j++) m.grid[5][j] = 1;
        for(int j=2; j<14; j++) m.grid[10][j] = 1;
        for(int i=2; i<8; i++) m.grid[i][2] = 1;
    }
}

int main() {
    int choice;
    // 隐藏光标，使动画更平滑
    CONSOLE_CURSOR_INFO cursor_info = {1, 0}; 
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);

    while(1) {
        system("cls");
        printf("=== 迷宫探索系统 ===\n");
        printf("1. 小型迷宫 (5x5)\n");
        printf("2. 中型迷宫 (10x10)\n");
        printf("3. 大型迷宫 (15x20)\n");
        printf("0. 退出\n");
        printf("请选择测试难度: ");
        scanf("%d", &choice);

        if (choice == 0) break;
        if (choice < 1 || choice > 3) continue;

        initMaze(choice);
        system("cls"); // 清屏开始动画
        
        printf("起点: (%d,%d), 终点: (%d,%d)\n", m.startX, m.startY, m.endX, m.endY);
        printf("按回车键开始探索...\n");
        getchar(); getchar();

        if (dfs(m.startX, m.startY)) {
            // 将光标移到底部
            COORD pos = {0, m.rows + 2};
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
            printf("\n恭喜！成功找到出口！(· 代表路径)\n");
        } else {
            COORD pos = {0, m.rows + 2};
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
            printf("\n遗憾！这是一个没有出路的死迷宫。\n");
        }

        printf("按回车返回菜单...");
        getchar();
    }
    return 0;
}