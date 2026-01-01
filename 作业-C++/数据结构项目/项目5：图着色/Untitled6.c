#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_VERTICES 50

// 定义图结构（邻接矩阵）
typedef struct {
    int adjMatrix[MAX_VERTICES][MAX_VERTICES];
    int numVertices;
    int degrees[MAX_VERTICES]; // 存储每个顶点的度数
} Graph;

// 颜色名称映射（用于显示）
const char *COLOR_NAMES[] = {
    "Red", "Green", "Blue", "Yellow", "Orange", "Purple", 
    "Cyan", "Magenta", "Lime", "Pink", "Brown", "Grey"
};

// 结构体用于排序顶点
typedef struct {
    int id;
    int degree;
} VertexInfo;

// 初始化图
void initGraph(Graph *g, int vertices) {
    g->numVertices = vertices;
    for (int i = 0; i < vertices; i++) {
        g->degrees[i] = 0;
        for (int j = 0; j < vertices; j++) {
            g->adjMatrix[i][j] = 0;
        }
    }
}

// 添加边（无向图）
void addEdge(Graph *g, int src, int dest) {
    if (src >= g->numVertices || dest >= g->numVertices) return;
    
    // 防止重复添加
    if (g->adjMatrix[src][dest] == 0) {
        g->adjMatrix[src][dest] = 1;
        g->adjMatrix[dest][src] = 1;
        g->degrees[src]++;
        g->degrees[dest]++;
    }
}

// 排序比较函数（按度数降序）
int compareVertices(const void *a, const void *b) {
    VertexInfo *v1 = (VertexInfo *)a;
    VertexInfo *v2 = (VertexInfo *)b;
    return v2->degree - v1->degree; // 降序
}

// 生成 Graphviz 可视化代码
void printGraphviz(Graph *g, int colors[]) {
    printf("\n========= 可视化代码 (Graphviz DOT) =========\n");
    printf("复制以下内容到 http://www.webgraphviz.com/ 查看图形化结果：\n\n");
    printf("graph G {\n");
    printf("  node [style=filled];\n");
    for (int i = 0; i < g->numVertices; i++) {
        // 获取颜色名，如果超出预定义范围则使用默认
        const char *cName = (colors[i] < 12) ? COLOR_NAMES[colors[i]] : "White";
        printf("  %d [fillcolor=%s, label=\"V%d(C%d)\"];\n", i, cName, i, colors[i]);
    }
    for (int i = 0; i < g->numVertices; i++) {
        for (int j = i + 1; j < g->numVertices; j++) {
            if (g->adjMatrix[i][j]) {
                printf("  %d -- %d;\n", i, j);
            }
        }
    }
    printf("}\n");
    printf("=============================================\n");
}

// 核心算法：Welsh-Powell 图着色
void graphColoring(Graph *g) {
    int result[MAX_VERTICES]; // 存储每个顶点分配的颜色索引
    bool available[MAX_VERTICES]; // 标记颜色是否可用
    VertexInfo sortedVertices[MAX_VERTICES];

    // 1. 初始化
    for (int i = 0; i < g->numVertices; i++) {
        result[i] = -1;   // -1 表示未着色
        available[i] = true; // true 表示颜色i可用
        sortedVertices[i].id = i;
        sortedVertices[i].degree = g->degrees[i];
    }

    // 2. 预处理：按度数从大到小排序顶点 (Welsh-Powell 策略)
    qsort(sortedVertices, g->numVertices, sizeof(VertexInfo), compareVertices);

    printf("\n[着色过程日志]\n");
    printf("顶点处理顺序(按度数降序): ");
    for(int i=0; i<g->numVertices; i++) printf("%d(d:%d) ", sortedVertices[i].id, sortedVertices[i].degree);
    printf("\n");

    // 3. 贪心着色
    int totalColorsUsed = 0;
    
    // 遍历排序后的每个顶点
    for (int i = 0; i < g->numVertices; i++) {
        int u = sortedVertices[i].id; // 当前要着色的真实顶点ID

        // 重置available数组
        for(int k=0; k<g->numVertices; k++) available[k] = true;

        // 检查 u 的所有邻居
        for (int v = 0; v < g->numVertices; v++) {
            if (g->adjMatrix[u][v] && result[v] != -1) {
                // 如果邻居 v 已经着色，则该颜色不可用于 u
                available[result[v]] = false;
            }
        }

        // 寻找第一个可用的颜色
        int cr;
        for (cr = 0; cr < g->numVertices; cr++) {
            if (available[cr]) {
                break;
            }
        }

        result[u] = cr; // 分配颜色
        if (cr + 1 > totalColorsUsed) {
            totalColorsUsed = cr + 1;
        }
    }

    // 4. 输出结果
    printf("\n[着色结果]\n");
    for (int i = 0; i < g->numVertices; i++) {
        printf("顶点 %d -> 颜色 %d (%s)\n", i, result[i], 
               (result[i] < 12) ? COLOR_NAMES[result[i]] : "Unknown");
    }
    printf("共使用颜色总数: %d\n", totalColorsUsed);

    // 5. 生成可视化数据
    printGraphviz(g, result);
}

// 预设场景构建
void loadScenario(Graph *g, int choice) {
    if (choice == 1) { 
        // 场景1: 简单三角形 (K3)
        initGraph(g, 3);
        addEdge(g, 0, 1); addEdge(g, 1, 2); addEdge(g, 2, 0);
    } 
    else if (choice == 2) { 
        // 场景2: 典型的二部图 (Bipartite Graph) - 应该只需要2种颜色
        // 0-1, 0-3, 2-1, 2-3 (Square)
        initGraph(g, 4);
        addEdge(g, 0, 1); addEdge(g, 0, 3);
        addEdge(g, 2, 1); addEdge(g, 2, 3);
    }
    else if (choice == 3) {
        // 场景3: 比较复杂的图 (Peterson Graph 简化版或其他)
        // 一个中心点连着周围所有点 (Star Graph)，中心点度数高
        initGraph(g, 6);
        addEdge(g, 0, 1); addEdge(g, 0, 2); addEdge(g, 0, 3); 
        addEdge(g, 0, 4); addEdge(g, 0, 5);
        // 再加一些外围连接
        addEdge(g, 1, 2); addEdge(g, 3, 4);
    }
    else if (choice == 4) {
        // 场景4: 完全图 (K5) - 每个点都相连，需要5种颜色
        initGraph(g, 5);
        for(int i=0; i<5; i++)
            for(int j=i+1; j<5; j++)
                addEdge(g, i, j);
    }
}

int main() {
    Graph g;
    int choice;

    while(1) {
        printf("\n######## 图着色算法演示 ########\n");
        printf("1. 简单三角形 (3顶点)\n");
        printf("2. 环状二部图 (4顶点)\n");
        printf("3. 星型复杂图 (6顶点)\n");
        printf("4. 完全图 K5 (5顶点)\n");
        printf("0. 退出\n");
        printf("请选择测试图结构: ");
        scanf("%d", &choice);

        if (choice == 0) break;
        if (choice < 1 || choice > 4) continue;

        loadScenario(&g, choice);
        graphColoring(&g);
        
        printf("\n按回车键继续...");
        getchar(); getchar();
    }

    return 0;
}