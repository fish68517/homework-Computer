
import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.*;
import static org.junit.jupiter.api.Assertions.assertEquals;

class DataProcessorTest {

    DataProcessor processor = new DataProcessor();

    // 测试用例1: 测试 "High" 分类的下边界值
    @Test
    void testCategorizeValue_High_Boundary() {
        assertEquals("High", processor.categorizeValue(101));
    }

    // 测试用例2: 测试 "High" 分类的典型值
    @Test
    void testCategorizeValue_High_Typical() {
        assertEquals("High", processor.categorizeValue(200));
    }

    // 测试用例3: 测试 "Medium" 分类的上边界值
    @Test
    void testCategorizeValue_Medium_UpperBoundary() {
        assertEquals("Medium", processor.categorizeValue(100));
    }

    // 测试用例4: 测试 "Medium" 分类的下边界值
    @Test
    void testCategorizeValue_Medium_LowerBoundary() {
        // 由于源代码中存在bug (value > 50)，这个测试将会失败。
        // 这正是单元测试的意义所在——发现代码与需求不符的bug。
        // 一份好的测试报告应该指出这个失败。
        assertEquals("Medium", processor.categorizeValue(51));
    }

    // 测试用例5: 测试 "Low" 分类的上边界值
    @Test
    void testCategorizeValue_Low_Boundary() {
        assertEquals("Low", processor.categorizeValue(50));
    }

    // 测试用例6: 测试 "Low" 分类的典型值
    @Test
    void testCategorizeValue_Low_Typical() {
        assertEquals("Low", processor.categorizeValue(0));
    }
}