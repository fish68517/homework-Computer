#include <stdio.h>
#include <stdlib.h>

#define MAX_STATIONS 100

// 定义加油站结构体
typedef struct {
    double price;    // 油价
    double distance; // 距离起点的距离
} Station;

// 比较函数，用于按照距离对加油站进行排序
int compareStations(const void *a, const void *b) {
    Station *s1 = (Station *)a;
    Station *s2 = (Station *)b;
    if (s1->distance > s2->distance) return 1;
    else if (s1->distance < s2->distance) return -1;
    else return 0;
}

int main() {
    double C_max;       // 油箱最大容量
    double D_total;     // 目的地总距离
    double D_avg;       // 每单位油能跑的距离 (km/L)
    double P_start;     // 起始点油价 (假设起点也是个加油站，或者是初始补给点)
    int N;              // 沿途加油站数量 (不含起点和终点)

    Station stations[MAX_STATIONS];

    printf("######## 经济出行加油助手 ########\n");
    
    // 1. 输入基本信息
    printf("请输入油箱最大容量 (升): ");
    scanf("%lf", &C_max);
    printf("请输入目的地总距离 (千米): ");
    scanf("%lf", &D_total);
    printf("请输入每升油行驶距离 (千米/升): ");
    scanf("%lf", &D_avg);
    printf("请输入起点当前油价 (元/升): ");
    scanf("%lf", &P_start); // 如果初始有油，此价格可理解为在起点补满的成本，或设为0
    printf("请输入沿途加油站数量: ");
    scanf("%d", &N);

    // 2. 输入加油站信息
    stations[0].distance = 0.0;
    stations[0].price = P_start;

    printf("请依次输入 %d 个加油站的 [距离起点距离] 和 [油价]:\n", N);
    for (int i = 1; i <= N; i++) {
        printf("加油站 %d: ", i);
        scanf("%lf %lf", &stations[i].distance, &stations[i].price);
    }

    // 将终点作为一个虚拟加油站放入数组，价格设为0（因为到了终点就不需要加油了，价格最低最诱人）
    stations[N + 1].distance = D_total;
    stations[N + 1].price = 0.0;

    // 按距离排序，保证逻辑顺序
    qsort(stations, N + 2, sizeof(Station), compareStations);

    // 3. 贪心算法核心逻辑
    double currentGas = 0.0;   // 当前油量 (假设初始为空，或者可以在起点加满)
    // 如果题目要求初始可以是满的，可以修改这里。
    // 本程序逻辑假设：在起点（距离0）处，我们根据策略决定加多少油。
    
    double totalCost = 0.0;    // 总花费
    int currentStation = 0;    // 当前所在的站点索引
    double maxDistance = C_max * D_avg; // 满油最大行驶距离
    int fillCount = 0;         // 加油次数

    // 检查起点是否就已经不可达第一站（或者第一站太远）
    if (stations[1].distance - stations[0].distance > maxDistance) {
        printf("\n错误：出发点距离第一个加油站太远，无法启动！\n");
        printf("最大行驶距离: X = %.2f km\n", maxDistance);
        return 0;
    }

    while (currentStation < N + 1) {
        // 当前站点的油价和距离
        double currPrice = stations[currentStation].price;
        double currDist = stations[currentStation].distance;

        // 在当前位置，满油能到达的最远距离范围内，寻找加油站
        int bestNextStation = -1;
        double minPriceInrange = 100000.0; // 一个足够大的数
        int minPriceStationIndex = -1;

        // 遍历所有满油能到达的站点
        int foundCheaper = 0;
        for (int i = currentStation + 1; i <= N + 1 && stations[i].distance - currDist <= maxDistance; i++) {
            // 策略 A: 如果找到第一个比当前站便宜（或相等）的站
            if (stations[i].price < currPrice) {
                bestNextStation = i;
                foundCheaper = 1;
                break; // 只要找到第一个便宜的，就去那里，不再往后看
            }
            
            // 记录范围内最便宜的站（虽然比当前站贵，但在所有贵的里面选个最便宜的）
            if (stations[i].price < minPriceInrange) {
                minPriceInrange = stations[i].price;
                minPriceStationIndex = i;
            }
        }

        // 判断是否无路可走
        if (foundCheaper == 0 && minPriceStationIndex == -1) {
            // 范围内没有任何站点（包括终点）
            double actualMaxReach = currDist + maxDistance;
            printf("\n无法到达目的地！\n");
            printf("在 %.2f km 处耗尽燃油，前方无加油站。\n", currDist);
            printf("最大行驶距离: X = %.2f km\n", actualMaxReach);
            return 0;
        }

        if (foundCheaper) {
            // === 策略 1: 前方有比当前更便宜的站 ===
            // 动作：只需要把油加到刚好能去那个便宜站即可（如果油够就不加）
            double distNeed = stations[bestNextStation].distance - currDist;
            double gasNeed = distNeed / D_avg;

            if (currentGas >= gasNeed) {
                // 油够，直接去，不加油
                currentGas -= gasNeed;
            } else {
                // 油不够，加到刚好够
                double addGas = gasNeed - currentGas;
                totalCost += addGas * currPrice;
                fillCount++;
                currentGas = 0; // 到了那里刚好用完
                printf("在 %.1f km 处加油 %.2f 升 (单价 %.2f)，花费 %.2f 元。前往 %.1f km 处。\n", 
                       currDist, addGas, currPrice, addGas * currPrice, stations[bestNextStation].distance);
            }
            currentStation = bestNextStation;
        } else {
            // === 策略 2: 前方没有比当前便宜的站 ===
            // 动作：在当前这个“相对便宜”的站把油加满！
            // 然后开到范围内“价格最低”的那个站（虽然比当前贵，但没办法，总得往前走）
            
            // 特例：如果终点就在范围内，直接去终点，不需要加满
            if (stations[minPriceStationIndex].distance == D_total) {
                double distNeed = stations[minPriceStationIndex].distance - currDist;
                double gasNeed = distNeed / D_avg;
                 if (currentGas < gasNeed) {
                    double addGas = gasNeed - currentGas;
                    totalCost += addGas * currPrice;
                    fillCount++;
                    printf("在 %.1f km 处补足 %.2f 升以到达终点。\n", currDist, addGas);
                 }
                 currentStation = minPriceStationIndex;
            } else {
                // 真的是要去下一个中间站
                double addGas = C_max - currentGas; // 加满
                totalCost += addGas * currPrice;
                fillCount++;
                currentGas = C_max; // 现在满油
                
                double distToNext = stations[minPriceStationIndex].distance - currDist;
                currentGas -= distToNext / D_avg; // 开到下一站消耗的油
                
                printf("在 %.1f km 处将油箱加满 (%.2f 升)，花费 %.2f 元。前往 %.1f km 处。\n", 
                       currDist, addGas, addGas * currPrice, stations[minPriceStationIndex].distance);
                       
                currentStation = minPriceStationIndex;
            }
        }
    }

    printf("\n======== 旅程结束 ========\n");
    printf("成功到达目的地！\n");
    printf("加油总次数: %d 次\n", fillCount);
    printf("加油总成本: %.2f 元\n", totalCost);

    // 暂停查看结果
    getchar(); getchar();
    return 0;
}