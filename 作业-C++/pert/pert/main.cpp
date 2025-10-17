#include "PERT.h"

int main() {
    PERT pert;
    int choice;
    cout << "选择输入方式：1-键盘输入 2-文本输入" << endl;
    cin >> choice;

    // 1. 输入数据
    if (choice == 1) {
        pert.inputFromKeyboard();
    } else if (choice == 2) {
        string filePath;
        cout << "输入文本文件路径（如input.txt）：";
        cin >> filePath;
        pert.inputFromFile(filePath);
    } else {
        cout << "输入错误！" << endl;
        return 0;
    }

    // 2. 核心计算（顺序不可变）
    pert.calculateTeSigma2();
    pert.calculateES_EF();
    pert.calculateLS_LF();
    pert.findCriticalPath();

    // 3. 输出结果
    pert.outputResult();

    return 0;
}