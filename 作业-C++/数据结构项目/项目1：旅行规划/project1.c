#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX_CITIES 20
#define INF 999999  // 定义无穷大，表示不可达

// 定义路线信息的结构体
typedef struct {
    int cost;       // 费用
    int time;       // 时间（小时）
    int type;       // 交通类型：0-无通路, 1-飞机, 2-火车
} Route;

// 定义图的结构体
typedef struct {
    char cityNames[MAX_CITIES][20]; // 存储城市名称
    Route matrix[MAX_CITIES][MAX_CITIES]; // 邻接矩阵
    int numCities; // 当前城市数量
} Graph;

// 全局图变量
Graph g;

// 初始化图数据（为了演示，这里预设了一些数据）
void initGraph() {
    g.numCities = 5;
    strcpy(g.cityNames[0], "北京");
    strcpy(g.cityNames[1], "上海");
    strcpy(g.cityNames[2], "西安");
    strcpy(g.cityNames[3], "成都");
    strcpy(g.cityNames[4], "广州");

    // 初始化所有路径为无穷大
    for (int i = 0; i < MAX_CITIES; i++) {
        for (int j = 0; j < MAX_CITIES; j++) {
            g.matrix[i][j].cost = INF;
            g.matrix[i][j].time = INF;
            g.matrix[i][j].type = 0;
        }
    }

    // 手动添加一些路线 (双向)
    // 北京(0) - 上海(1): 飞机, 费用1000, 时间2h
    g.matrix[0][1].cost = 1000; g.matrix[0][1].time = 2; g.matrix[0][1].type = 1;
    g.matrix[1][0].cost = 1000; g.matrix[1][0].time = 2; g.matrix[1][0].type = 1;

    // 北京(0) - 西安(2): 火车, 费用300, 时间6h
    g.matrix[0][2].cost = 300; g.matrix[0][2].time = 6; g.matrix[0][2].type = 2;
    g.matrix[2][0].cost = 300; g.matrix[2][0].time = 6; g.matrix[2][0].type = 2;

    // 西安(2) - 成都(3): 火车, 费用200, 时间4h
    g.matrix[2][3].cost = 200; g.matrix[2][3].time = 4; g.matrix[2][3].type = 2;
    g.matrix[3][2].cost = 200; g.matrix[3][2].time = 4; g.matrix[3][2].type = 2;

    // 上海(1) - 广州(4): 飞机, 费用1200, 时间3h
    g.matrix[1][4].cost = 1200; g.matrix[1][4].time = 3; g.matrix[1][4].type = 1;
    g.matrix[4][1].cost = 1200; g.matrix[4][1].time = 3; g.matrix[4][1].type = 1;
    
    // 成都(3) - 广州(4): 火车, 费用500, 时间10h
    g.matrix[3][4].cost = 500; g.matrix[3][4].time = 10; g.matrix[3][4].type = 2;
    g.matrix[4][3].cost = 500; g.matrix[4][3].time = 10; g.matrix[4][3].type = 2;

    // 北京(0) - 广州(4): 直飞, 费用2000, 时间3h
    g.matrix[0][4].cost = 2000; g.matrix[0][4].time = 3; g.matrix[0][4].type = 1;
    g.matrix[4][0].cost = 2000; g.matrix[4][0].time = 3; g.matrix[4][0].type = 1;
}

// 根据城市名获取索引
int getCityIndex(char *name) {
    for (int i = 0; i < g.numCities; i++) {
        if (strcmp(g.cityNames[i], name) == 0) {
            return i;
        }
    }
    return -1;
}

// 打印路径
void printPath(int parent[], int j) {
    if (parent[j] == -1) return;
    printPath(parent, parent[j]);
    printf(" -> %s", g.cityNames[j]);
}

// Dijkstra算法核心
// strategy: 1-最短时间, 2-最低成本, 3-最少中转
void dijkstra(int startNode, int endNode, int strategy) {
    int dist[MAX_CITIES];    // 存储起始点到各点的最短距离（权重）
    int visited[MAX_CITIES]; // 标记是否已访问
    int parent[MAX_CITIES];  // 存储路径前驱节点

    // 初始化
    for (int i = 0; i < g.numCities; i++) {
        dist[i] = INF;
        visited[i] = 0;
        parent[i] = -1;
    }
    dist[startNode] = 0;

    for (int count = 0; count < g.numCities - 1; count++) {
        // 1. 寻找未访问节点中dist最小的节点
        int min = INF, u = -1;
        for (int v = 0; v < g.numCities; v++) {
            if (!visited[v] && dist[v] <= min) {
                min = dist[v];
                u = v;
            }
        }

        if (u == -1 || u == endNode) break; // 无法到达或已找到终点
        visited[u] = 1;

        // 2. 更新相邻节点的距离
        for (int v = 0; v < g.numCities; v++) {
            // 如果存在连接且未访问
            if (!visited[v] && g.matrix[u][v].type != 0) {
                int weight = 0;
                
                // 根据策略决定权重
                if (strategy == 1) weight = g.matrix[u][v].time;       // 时间
                else if (strategy == 2) weight = g.matrix[u][v].cost;  // 成本
                else if (strategy == 3) weight = 1;                    // 中转次数(每条边权重为1)

                if (dist[u] != INF && dist[u] + weight < dist[v]) {
                    dist[v] = dist[u] + weight;
                    parent[v] = u;
                }
            }
        }
    }

    // 输出结果
    if (dist[endNode] == INF) {
        printf("\n抱歉，从 %s 到 %s 没有可达的路线。\n", g.cityNames[startNode], g.cityNames[endNode]);
    } else {
        printf("\n================ 推荐路线 ================\n");
        printf("起点: %s  终点: %s\n", g.cityNames[startNode], g.cityNames[endNode]);
        printf("策略: %s\n", strategy == 1 ? "最短时间" : (strategy == 2 ? "最低成本" : "最少中转"));
        
        printf("路线详情: %s", g.cityNames[startNode]);
        printPath(parent, endNode);
        
        printf("\n\n统计数据:\n");
        // 这里需要重新回溯计算具体的总时间和总费用，因为dist只存了其中一种
        int totalCost = 0;
        int totalTime = 0;
        int curr = endNode;
        int transfers = 0;
        while(parent[curr] != -1) {
            int prev = parent[curr];
            totalCost += g.matrix[prev][curr].cost;
            totalTime += g.matrix[prev][curr].time;
            transfers++;
            curr = prev;
        }
        
        printf("总费用: %d 元\n", totalCost);
        printf("总时间: %d 小时\n", totalTime);
        printf("中转次数: %d 次 (乘坐交通工具 %d 次)\n", transfers > 0 ? transfers - 1 : 0, transfers);
        printf("==========================================\n");
    }
}

int main() {
    initGraph();
    char startName[20], endName[20];
    int choice;

    while (1) {
        printf("\n######## 旅行信息顾问系统 ########\n");
        printf("当前可用城市: ");
        for(int i=0; i<g.numCities; i++) printf("%s ", g.cityNames[i]);
        printf("\n");
        
        printf("请输入起点城市: ");
        scanf("%s", startName);
        int start = getCityIndex(startName);

        printf("请输入终点城市: ");
        scanf("%s", endName);
        int end = getCityIndex(endName);

        if (start == -1 || end == -1) {
            printf("错误：输入的城市不存在，请检查拼写。\n");
            continue;
        }

        printf("\n请选择偏好:\n");
        printf("1. 最短旅行时间\n");
        printf("2. 最低旅行成本\n");
        printf("3. 最少转换次数\n");
        printf("0. 退出系统\n");
        printf("请输入选项: ");
        scanf("%d", &choice);

        if (choice == 0) break;
        if (choice < 1 || choice > 3) {
            printf("无效选项！\n");
            continue;
        }

        dijkstra(start, end, choice);
        
        printf("\n按回车键继续...");
        getchar(); getchar(); // 暂停
    }
    return 0;
}