#include <cmath>
#include <iomanip>
#include <iostream>
#include <stdexcept>

// 抽象图形基类，规定所有图形都必须提供名称、面积和周长。
class Shape {
public:
    // 基类通过指针删除派生类对象时，需要虚析构函数。
    virtual ~Shape() {}

    // 纯虚函数使 Shape 成为抽象类，不能直接创建 Shape 对象。
    virtual const char* name() const = 0;
    virtual double area() const = 0;
    virtual double perimeter() const = 0;
};

// 圆形类继承 Shape，并实现基类规定的全部纯虚函数。
class Circle : public Shape {
public:
    explicit Circle(double radius) : radius_(radius) {
        // 半径必须为正数。
        if (radius_ <= 0) {
            throw std::invalid_argument("radius must be greater than zero");
        }
    }

    // override 表明该函数重写了基类中的虚函数。
    const char* name() const override {
        return "Circle";
    }

    double area() const override {
        const double pi = 3.14159265358979323846;
        return pi * radius_ * radius_;
    }

    double perimeter() const override {
        const double pi = 3.14159265358979323846;
        return 2 * pi * radius_;
    }

private:
    // 半径设为私有成员，体现数据封装。
    double radius_;
};

// 矩形类使用长和宽计算面积及周长。
class Rectangle : public Shape {
public:
    Rectangle(double length, double width) : length_(length), width_(width) {
        // 长和宽都必须为正数。
        if (length_ <= 0 || width_ <= 0) {
            throw std::invalid_argument("length and width must be greater than zero");
        }
    }

    const char* name() const override {
        return "Rectangle";
    }

    double area() const override {
        return length_ * width_;
    }

    double perimeter() const override {
        return 2 * (length_ + width_);
    }

private:
    // 外部不能直接修改矩形的长和宽。
    double length_;
    double width_;
};

// 三角形类使用三条边保存图形数据。
class Triangle : public Shape {
public:
    Triangle(double sideA, double sideB, double sideC)
        : sideA_(sideA), sideB_(sideB), sideC_(sideC) {
        // 三边必须为正数，并且任意两边之和必须大于第三边。
        if (sideA_ <= 0 || sideB_ <= 0 || sideC_ <= 0 ||
            sideA_ + sideB_ <= sideC_ ||
            sideA_ + sideC_ <= sideB_ ||
            sideB_ + sideC_ <= sideA_) {
            throw std::invalid_argument("the three sides cannot form a triangle");
        }
    }

    const char* name() const override {
        return "Triangle";
    }

    double area() const override {
        // 使用海伦公式计算三角形面积。
        const double halfPerimeter = perimeter() / 2;
        return std::sqrt(
            halfPerimeter *
            (halfPerimeter - sideA_) *
            (halfPerimeter - sideB_) *
            (halfPerimeter - sideC_));
    }

    double perimeter() const override {
        return sideA_ + sideB_ + sideC_;
    }

private:
    double sideA_;
    double sideB_;
    double sideC_;
};

int main() {
    double radius;
    double length;
    double width;
    double sideA;
    double sideB;
    double sideC;

    // 输入圆的半径。
    std::cout << "Enter the circle radius: ";
    if (!(std::cin >> radius)) {
        std::cerr << "Invalid input.\n";
        return 1;
    }

    // 输入矩形的长和宽，中间使用空格分隔。
    std::cout << "Enter the rectangle length and width: ";
    if (!(std::cin >> length >> width)) {
        std::cerr << "Invalid input.\n";
        return 1;
    }

    // 输入三角形的三条边，中间使用空格分隔。
    std::cout << "Enter the three sides of the triangle: ";
    if (!(std::cin >> sideA >> sideB >> sideC)) {
        std::cerr << "Invalid input.\n";
        return 1;
    }

    try {
        // 根据输入的数据分别创建三个派生类对象。
        Circle circle(radius);
        Rectangle rectangle(length, width);
        Triangle triangle(sideA, sideB, sideC);

        // 使用基类指针调用派生类重写的函数。
        Shape* shapes[] = {&circle, &rectangle, &triangle};

        std::cout << std::fixed << std::setprecision(2);
        for (const Shape* shape : shapes) {
            // 实际调用哪个类的函数由指针所指对象决定，实现动态绑定。
            std::cout << "\n" << shape->name() << '\n';
            std::cout << "Area: " << shape->area() << '\n';
            std::cout << "Perimeter: " << shape->perimeter() << '\n';
        }
    }
    catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
