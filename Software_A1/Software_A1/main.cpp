#include <iostream>
#include <stdexcept>

// Rational 类用于表示有理数，并保证分数始终为最简形式。
class Rational {
public:
    // 构造函数重载：默认值、整数和分数。
    // 默认构造函数将有理数初始化为 0/1。
    Rational() : numerator_(0), denominator_(1) {}

    // 单参数构造函数将整数转换为分母为 1 的有理数。
    Rational(long long value) : numerator_(value), denominator_(1) {}

    // 双参数构造函数接收分子和分母，并自动进行规范化。
    Rational(long long numerator, long long denominator)
        : numerator_(numerator), denominator_(denominator) {
        normalize();
    }

    // 重载加法运算符：先通分，再将分子相加。
    Rational operator+(const Rational& other) const {
        return Rational(
            numerator_ * other.denominator_ + other.numerator_ * denominator_,
            denominator_ * other.denominator_);
    }

    // 重载减法运算符：先通分，再将分子相减。
    Rational operator-(const Rational& other) const {
        return Rational(
            numerator_ * other.denominator_ - other.numerator_ * denominator_,
            denominator_ * other.denominator_);
    }

    // 重载乘法运算符：分子相乘，分母相乘。
    Rational operator*(const Rational& other) const {
        return Rational(
            numerator_ * other.numerator_,
            denominator_ * other.denominator_);
    }

    // 重载除法运算符：乘以另一个有理数的倒数。
    Rational operator/(const Rational& other) const {
        // 除数为零时不能进行除法。
        if (other.numerator_ == 0) {
            throw std::runtime_error("division by zero");
        }
        return Rational(
            numerator_ * other.denominator_,
            denominator_ * other.numerator_);
    }

    // 判断当前有理数是否为零。
    bool isZero() const {
        return numerator_ == 0;
    }

    // 重载输出运算符，使 Rational 对象可以直接使用 cout 输出。
    // 分母为 1 时只输出整数，否则输出“分子/分母”。
    friend std::ostream& operator<<(std::ostream& output, const Rational& value) {
        output << value.numerator_;
        if (value.denominator_ != 1) {
            output << '/' << value.denominator_;
        }
        return output;
    }

private:
    // 私有数据不能被外部随意修改，体现封装和数据安全性。
    long long numerator_;
    long long denominator_;

    // 使用欧几里得算法求最大公约数。
    static long long gcd(long long first, long long second) {
        // 最大公约数只与数值大小有关，因此先去掉分子的负号。
        if (first < 0) {
            first = -first;
        }

        // 不断用余数替换两个数，直到余数为零。
        while (second != 0) {
            const long long remainder = first % second;
            first = second;
            second = remainder;
        }
        return first;
    }

    // 检查分母、统一符号并完成约分。
    void normalize() {
        // 分母为零时，有理数没有意义，直接抛出异常。
        if (denominator_ == 0) {
            throw std::invalid_argument("denominator cannot be zero");
        }

        // 将负号统一放在分子上。
        if (denominator_ < 0) {
            numerator_ = -numerator_;
            denominator_ = -denominator_;
        }

        // 使用最大公约数将分数约为最简形式。
        const long long divisor = gcd(numerator_, denominator_);
        numerator_ /= divisor;
        denominator_ /= divisor;
    }
};

int main() {
    // 分别保存两个有理数的分子和分母。
    long long firstNumerator;
    long long firstDenominator;
    long long secondNumerator;
    long long secondDenominator;

    // 输入格式为“分子 分母”，两个整数之间使用空格分隔。
    std::cout << "Enter the first rational (numerator denominator): ";
    if (!(std::cin >> firstNumerator >> firstDenominator)) {
        std::cerr << "Invalid input.\n";
        return 1;
    }

    // 输入第二个有理数。
    std::cout << "Enter the second rational (numerator denominator): ";
    if (!(std::cin >> secondNumerator >> secondDenominator)) {
        std::cerr << "Invalid input.\n";
        return 1;
    }

    try {
        // 创建对象时会自动检查分母并将分数约为最简形式。
        const Rational first(firstNumerator, firstDenominator);
        const Rational second(secondNumerator, secondDenominator);

        // 依次计算并输出加、减、乘的结果。
        std::cout << "\n" << first << " + " << second << " = " << first + second << '\n';
        std::cout << first << " - " << second << " = " << first - second << '\n';
        std::cout << first << " * " << second << " = " << first * second << '\n';

        // 第二个数为零时不执行除法，避免除零错误。
        if (second.isZero()) {
            std::cout << first << " / " << second << " = undefined\n";
        }
        else {
            std::cout << first << " / " << second << " = " << first / second << '\n';
        }
    }
    // 捕获零分母等异常，并输出错误原因。
    catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
