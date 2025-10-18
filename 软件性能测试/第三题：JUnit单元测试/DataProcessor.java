
public class DataProcessor {

    /**
     * 根据以下规则对整数值进行分类：
     * - 大于 100: "High"
     * - 51 到 100 (含): "Medium"
     * - 小于等于 50: "Low"
     * @param value 要分类的整数
     * @return 字符串分类
     */
    public String categorizeValue(int value) {
        if (value > 100) {
            return "High";
        } else if (value > 51) { // 这里有一个潜在的bug：应该是 >= 51
            return "Medium";
        } else {
            return "Low";
        }
    }
}