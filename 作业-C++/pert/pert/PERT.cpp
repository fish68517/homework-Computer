#include "PERT.h"

// 辅助：通过活动名称找索引（找不到返回-1）
int PERT::findActivityIndex(string name) {
    for (int i = 0; i < activities.size(); i++) {
        if (activities[i].name == name) return i;
    }
    return -1;
}

// 键盘输入：循环接收，输入"end"停止
void PERT::inputFromKeyboard() {
    Activity act;
    while (true) {
        cout << "输入活动名称（输入end结束）：";
        cin >> act.name;
        if (act.name == "end") break;

        // 输入紧前活动（用逗号分隔，如"A,B"）
        cout << "输入紧前活动（无则输0）：";
        string prevStr;
        cin >> prevStr;
        act.prev.clear();
        if (prevStr != "0") {
            int pos = 0;
            while ((pos = prevStr.find(',')) != -1) {
                act.prev.push_back(prevStr.substr(0, pos));
                prevStr.erase(0, pos + 1);
            }
            act.prev.push_back(prevStr);
        }

        // 输入a/m/b时间
        cout << "输入乐观时间a、最可能时间m、悲观时间b（空格分隔）：";
        cin >> act.a >> act.m >> act.b;
        activities.push_back(act);
    }
}

// 文本输入：读取指定格式的txt文件（每行：名称,紧前, a,m,b）
void PERT::inputFromFile(string filePath) {
    ifstream file(filePath);
    if (!file.is_open()) {
        cout << "文件打开失败！" << endl;
        return;
    }
    string line;
    while (getline(file, line)) {
        Activity act;
        int pos = 0;

        // 解析活动名称（第一个逗号前）
        pos = line.find(',');
        act.name = line.substr(0, pos);
        line.erase(0, pos + 1);

        // 解析紧前活动（第二个逗号前）
        pos = line.find(',');
        string prevStr = line.substr(0, pos);
        line.erase(0, pos + 1);
        act.prev.clear();
        if (prevStr != "0") {
            while ((pos = prevStr.find(',')) != -1) {
                act.prev.push_back(prevStr.substr(0, pos));
                prevStr.erase(0, pos + 1);
            }
            act.prev.push_back(prevStr);
        }

        // 解析a/m/b（剩余部分用空格分隔）
        pos = line.find(' ');
        act.a = stod(line.substr(0, pos));
        line.erase(0, pos + 1);
        pos = line.find(' ');
        act.m = stod(line.substr(0, pos));
        line.erase(0, pos + 1);
        act.b = stod(line);

        activities.push_back(act);
    }
    file.close();
    cout << "文件读取成功，共" << activities.size() << "个活动" << endl;
}// 计算每个活动的期望时间te和方差sigma2
void PERT::calculateTeSigma2() {
    for (int i = 0; i < activities.size(); i++) {
        activities[i].te = (activities[i].a + 4 * activities[i].m + activities[i].b) / 6;
        activities[i].sigma2 = pow((activities[i].b - activities[i].a) / 6, 2);
    }
    cout << "期望时间和方差计算完成" << endl;
}

// 计算最早开始（ES）和最早完成（EF=ES+te）：按拓扑顺序，从无紧前活动的活动开始
void PERT::calculateES_EF() {
    for (int i = 0; i < activities.size(); i++) {
        double maxEF = 0;
        // 找所有紧前活动的最大EF
        for (string prevName : activities[i].prev) {
            int prevIdx = findActivityIndex(prevName);
            if (prevIdx != -1 && activities[prevIdx].ef > maxEF) {
                maxEF = activities[prevIdx].ef;
            }
        }
        activities[i].es = maxEF;
        activities[i].ef = activities[i].es + activities[i].te;
    }
    cout << "ES和EF计算完成" << endl;
}

// 计算最晚完成（LF）和最晚开始（LS=LF-te）：逆序，从总EF最大的活动开始
void PERT::calculateLS_LF() {
    // 第一步：找总EF最大的值（项目最早完成时间，作为最后活动的LF）
    double maxTotalEF = 0;
    for (Activity act : activities) {
        if (act.ef > maxTotalEF) maxTotalEF = act.ef;
    }

    // 第二步：逆序计算每个活动的LF（取紧后活动的最小LS）
    for (int i = activities.size() - 1; i >= 0; i--) {
        double minLS = maxTotalEF;
        // 找当前活动的所有紧后活动（遍历所有活动，看谁的紧前包含当前活动）
        for (int j = 0; j < activities.size(); j++) {
            for (string prevName : activities[j].prev) {
                if (prevName == activities[i].name && activities[j].ls < minLS) {
                    minLS = activities[j].ls;
                }
            }
        }
        activities[i].lf = minLS;
        activities[i].ls = activities[i].lf - activities[i].te;
        activities[i].slack = activities[i].ls - activities[i].es; // 计算时差
    }
    cout << "LS、LF和时差计算完成" << endl;
}

// 找关键路径：时差为0的活动，按先后顺序排列
void PERT::findCriticalPath() {
    criticalPath.clear();
    // 第一步：找第一个关键活动（无紧前活动且时差为0）
    for (Activity act : activities) {
        if (act.prev.empty() && act.slack == 0) {
            criticalPath.push_back(act.name);
            break;
        }
    }

    // 第二步：按紧后关系找后续关键活动
    while (true) {
        string lastCritical = criticalPath.back();
        bool foundNext = false;
        // 遍历所有活动，找以lastCritical为紧前、且时差为0的活动
        for (Activity act : activities) {
            for (string prevName : act.prev) {
                if (prevName == lastCritical && act.slack == 0) {
                    // 避免重复添加（防止循环）
                    if (find(criticalPath.begin(), criticalPath.end(), act.name) == criticalPath.end()) {
                        criticalPath.push_back(act.name);
                        foundNext = true;
                        break;
                    }
                }
            }
            if (foundNext) break;
        }
        if (!foundNext) break; // 没有下一个关键活动，结束
    }
    cout << "关键路径查找完成" << endl;
}void PERT::outputResult() {
    // 1. 输出到控制台
    cout << "\n===== PERT模型计算结果 =====" << endl;
    // 输出活动列表（含所有参数）
    cout << "1. 活动详细信息：" << endl;
    cout << "名称\t紧前活动\t a\t m\t b\t te\t sigma2\t ES\t EF\t LS\t LF\t 时差" << endl;
    for (Activity act : activities) {
        cout << act.name << "\t";
        if (act.prev.empty()) cout << "无\t";
        else {
            for (int k = 0; k < act.prev.size(); k++) {
                cout << act.prev[k] << (k == act.prev.size()-1 ? "\t" : ",");
            }
        }
        printf("%.2f\t %.2f\t %.2f\t %.2f\t %.2f\t %.2f\t %.2f\t %.2f\t %.2f\t %d\n",
               act.a, act.m, act.b, act.te, act.sigma2,
               act.es, act.ef, act.ls, act.lf, act.slack);
    }
    // 输出关键路径
    cout << "\n2. 关键路径：";
    for (int i = 0; i < criticalPath.size(); i++) {
        cout << criticalPath[i] << (i == criticalPath.size()-1 ? "" : "→");
    }
    // 输出关键路径总时间（最后一个活动的EF）
    double criticalTime = activities[findActivityIndex(criticalPath.back())].ef;
    cout << "\n3. 关键路径总期望时间：" << criticalTime << "天" << endl;

    // 2. 输出到result.txt文件
    ofstream outFile("result.txt");
    if (outFile.is_open()) {
        outFile << "===== PERT模型计算结果 =====" << endl;
        outFile << "1. 活动详细信息：" << endl;
        outFile << "名称\t紧前活动\t a\t m\t b\t te\t sigma2\t ES\t EF\t LS\t LF\t 时差" << endl;
        for (Activity act : activities) {
            outFile << act.name << "\t";
            if (act.prev.empty()) outFile << "无\t";
            else {
                for (int k = 0; k < act.prev.size(); k++) {
                    outFile << act.prev[k] << (k == act.prev.size()-1 ? "\t" : ",");
                }
            }
            outFile << act.a << "\t" << act.m << "\t" << act.b << "\t" << act.te << "\t" << act.sigma2 << "\t"
                    << act.es << "\t" << act.ef << "\t" << act.ls << "\t" << act.lf << "\t" << act.slack << endl;
        }
        outFile << "\n2. 关键路径：";
        for (int i = 0; i < criticalPath.size(); i++) {
            outFile << criticalPath[i] << (i == criticalPath.size()-1 ? "" : "→");
        }
        outFile << "\n3. 关键路径总期望时间：" << criticalTime << "天" << endl;
        outFile.close();
        cout << "\n结果已保存到result.txt" << endl;
    } else {
        cout << "\n文件保存失败！" << endl;
    }
}