#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TREE_HT 100
#define MAX_BUFFER 10240 // 最大支持输入的字符数

// --- 数据结构定义 ---

// 哈夫曼树节点
struct MinHeapNode {
    char data;                  // 字符
    unsigned freq;              // 频率
    struct MinHeapNode *left, *right; // 左右子节点
};

// 最小堆（优先队列）
struct MinHeap {
    unsigned size;              // 当前堆大小
    unsigned capacity;          // 容量
    struct MinHeapNode** array; // 节点指针数组
};

// 全局变量存储编码表 (ASCII 256个字符, 每个编码最长100)
char huffmanCodes[256][MAX_TREE_HT]; 
struct MinHeapNode* rootGlobal = NULL; // 保存树根以便解压

// --- 辅助函数：创建节点 ---
struct MinHeapNode* newNode(char data, unsigned freq) {
    struct MinHeapNode* temp = (struct MinHeapNode*)malloc(sizeof(struct MinHeapNode));
    temp->left = temp->right = NULL;
    temp->data = data;
    temp->freq = freq;
    return temp;
}

// --- 最小堆操作 (核心逻辑) ---
struct MinHeap* createMinHeap(unsigned capacity) {
    struct MinHeap* minHeap = (struct MinHeap*)malloc(sizeof(struct MinHeap));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (struct MinHeapNode**)malloc(minHeap->capacity * sizeof(struct MinHeapNode*));
    return minHeap;
}

void swapMinHeapNode(struct MinHeapNode** a, struct MinHeapNode** b) {
    struct MinHeapNode* t = *a;
    *a = *b;
    *b = t;
}

void minHeapify(struct MinHeap* minHeap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < minHeap->size && minHeap->array[left]->freq < minHeap->array[smallest]->freq)
        smallest = left;
    if (right < minHeap->size && minHeap->array[right]->freq < minHeap->array[smallest]->freq)
        smallest = right;

    if (smallest != idx) {
        swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);
        minHeapify(minHeap, smallest);
    }
}

struct MinHeapNode* extractMin(struct MinHeap* minHeap) {
    struct MinHeapNode* temp = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];
    --minHeap->size;
    minHeapify(minHeap, 0);
    return temp;
}

void insertMinHeap(struct MinHeap* minHeap, struct MinHeapNode* minHeapNode) {
    ++minHeap->size;
    int i = minHeap->size - 1;
    while (i && minHeapNode->freq < minHeap->array[(i - 1) / 2]->freq) {
        minHeap->array[i] = minHeap->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    minHeap->array[i] = minHeapNode;
}

// --- 哈夫曼树构建 ---
struct MinHeapNode* buildHuffmanTree(char data[], int freq[], int size) {
    struct MinHeapNode *left, *right, *top;
    struct MinHeap* minHeap = createMinHeap(size);

    for (int i = 0; i < size; ++i)
        insertMinHeap(minHeap, newNode(data[i], freq[i]));

    while (minHeap->size != 1) {
        left = extractMin(minHeap);
        right = extractMin(minHeap);
        top = newNode('$', left->freq + right->freq); // '$' 是内部节点的占位符
        top->left = left;
        top->right = right;
        insertMinHeap(minHeap, top);
    }
    return extractMin(minHeap);
}

// --- 生成编码表 ---
void storeCodes(struct MinHeapNode* root, int arr[], int top) {
    if (root->left) {
        arr[top] = 0;
        storeCodes(root->left, arr, top + 1);
    }
    if (root->right) {
        arr[top] = 1;
        storeCodes(root->right, arr, top + 1);
    }
    // 如果是叶子节点，存储编码
    if (!(root->left) && !(root->right)) {
        int charIndex = (unsigned char)root->data;
        for (int i = 0; i < top; i++) {
            huffmanCodes[charIndex][i] = arr[i] + '0';
        }
        huffmanCodes[charIndex][top] = '\0'; // 字符串结束符
    }
}

// --- 主要功能函数 ---

// 1. 统计频率并构建树
void initHuffman(char* text) {
    int freq[256] = {0};
    int len = strlen(text);
    
    // 统计频率
    for(int i=0; i<len; i++) {
        freq[(unsigned char)text[i]]++;
    }

    // 准备构建数据
    char dataArr[256];
    int freqArr[256];
    int size = 0;
    
    for(int i=0; i<256; i++) {
        if(freq[i] > 0) {
            dataArr[size] = (char)i;
            freqArr[size] = freq[i];
            size++;
        }
    }

    // 构建树
    rootGlobal = buildHuffmanTree(dataArr, freqArr, size);
    
    // 生成编码表
    int arr[MAX_TREE_HT], top = 0;
    // 清空旧表
    memset(huffmanCodes, 0, sizeof(huffmanCodes));
    storeCodes(rootGlobal, arr, top);
}

// 2. 显示映射表
void printCodes() {
    printf("\n--- 字符编码映射表 ---\n");
    printf("字符\t| 频率\t| 哈夫曼编码\n");
    printf("------------------------\n");
    // 这里需要重新遍历一次树来打印，或者直接遍历所有ASCII
    // 为了简单，我们遍历编码表
    for(int i=0; i<256; i++) {
        if(strlen(huffmanCodes[i]) > 0) {
            if(i == '\n') printf("\\n\t| \t| %s\n", huffmanCodes[i]);
            else if(i == ' ') printf("Space\t| \t| %s\n", huffmanCodes[i]);
            else printf("%c\t| \t| %s\n", (char)i, huffmanCodes[i]);
        }
    }
    printf("------------------------\n");
}

// 3. 压缩 (String -> Bit String)
void compress(char* input) {
    printf("\n[压缩结果]: ");
    for(int i=0; i<strlen(input); i++) {
        printf("%s", huffmanCodes[(unsigned char)input[i]]);
    }
    printf("\n");
}

// 4. 解压缩 (Bit String -> String)
void decompress(char* bitString) {
    if (rootGlobal == NULL) {
        printf("错误：哈夫曼树未初始化，请先执行压缩步骤。\n");
        return;
    }
    
    struct MinHeapNode* curr = rootGlobal;
    printf("\n[解压结果]: ");
    for (int i = 0; i < strlen(bitString); i++) {
        if (bitString[i] == '0')
            curr = curr->left;
        else
            curr = curr->right;

        // 到达叶子节点
        if (curr->left == NULL && curr->right == NULL) {
            printf("%c", curr->data);
            curr = rootGlobal; // 重置回根节点
        }
    }
    printf("\n");
}

// 文件读取辅助
void readFile(char* buffer) {
    char filename[100];
    printf("请输入文件名: ");
    scanf("%s", filename);
    FILE* fp = fopen(filename, "r");
    if(!fp) {
        printf("文件打开失败！使用默认文本测试。\n");
        strcpy(buffer, "Hello World Data Structure");
    } else {
        // 简单读取，实际可能需要循环读取
        fgets(buffer, MAX_BUFFER, fp); 
        fclose(fp);
        // 去除fgets可能读入的换行
        buffer[strcspn(buffer, "\n")] = 0;
        printf("文件读取成功: %s\n", buffer);
    }
}

int main() {
    int choice;
    char textBuffer[MAX_BUFFER] = "";
    char bitBuffer[MAX_BUFFER * 8] = ""; // 压缩后的比特流可能变长

    while(1) {
        printf("\n######## 哈夫曼压缩系统 ########\n");
        printf("1. 输入字符串\n");
        printf("2. 读取文件\n");
        printf("3. 显示编码映射\n");
        printf("4. 执行压缩\n");
        printf("5. 执行解压缩\n");
        printf("0. 退出\n");
        printf("请选择: ");
        scanf("%d", &choice);
        getchar(); // 吃掉换行符

        switch(choice) {
            case 1:
                printf("请输入需压缩的文本: ");
                fgets(textBuffer, MAX_BUFFER, stdin);
                textBuffer[strcspn(textBuffer, "\n")] = 0; // 去换行
                initHuffman(textBuffer);
                printf("数据已加载并生成哈夫曼树。\n");
                break;
            case 2:
                readFile(textBuffer);
                initHuffman(textBuffer);
                break;
            case 3:
                if(rootGlobal) printCodes();
                else printf("请先输入数据(选项1或2)。\n");
                break;
            case 4:
                if(rootGlobal) compress(textBuffer);
                else printf("请先输入数据。\n");
                break;
            case 5:
                printf("请输入二进制位序列 (如 01011...): ");
                scanf("%s", bitBuffer);
                decompress(bitBuffer);
                break;
            case 0:
                exit(0);
            default:
                printf("无效输入。\n");
        }
    }
    return 0;
}