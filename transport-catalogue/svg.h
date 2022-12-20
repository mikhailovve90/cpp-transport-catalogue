#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <deque>
#include <set>
#define _USE_MATH_DEFINES
#include <cmath>
#include <variant>

namespace svg {

struct Rgb {
    Rgb() {};
    Rgb(uint8_t r, uint8_t g, uint8_t b) : red(r), green(g), blue(b) {
    }

    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;
};

struct Rgba : public Rgb {
    Rgba() {};
    Rgba(uint8_t r, uint8_t g, uint8_t b, double op) : Rgb::Rgb(r,g,b), opacity(op) {
    }
    double opacity = 1.0;
};

enum class StrokeLineCap {
    BUTT,
    ROUND,
    SQUARE,
};

enum class StrokeLineJoin {
    ARCS,
    BEVEL,
    MITER,
    MITER_CLIP,
    ROUND,
};

std::ostream& operator<<(std::ostream &os, const StrokeLineCap &stcp);
std::ostream& operator<<(std::ostream &os, const StrokeLineJoin &stlj);

//using Color = std::string;

using Color = std::variant<std::monostate, std::string, svg::Rgb, svg::Rgba> ;
inline const Color NoneColor{"none"};
std::ostream& operator<<(std::ostream &os, const Color &col);
std::ostream& operator<<(std::ostream &os, const std::optional<Color> &col);

void printColor(std::ostream& out, std::monostate);

void printColor(std::ostream& out, std::string name);

void printColor(std::ostream& out, svg::Rgb rgb);

void printColor(std::ostream& out, svg::Rgba& rgba);

std::string& text_to_svg_format(std::string& text);

template <typename Owner>
class PathProps {
  public:
    Owner& SetFillColor(Color color) {
        fill_color_ = std::move(color);
        return AsOwner();
    }
    Owner& SetStrokeColor(Color color) {
        stroke_color_ = std::move(color);
        return AsOwner();
    }

    Owner& SetStrokeWidth(double width) {
        stroke_width_ = std::move(width);
        return AsOwner();
    }

    Owner& SetStrokeLineCap(StrokeLineCap line_cap) {
        stroke_linecap_ = std::move(line_cap);
        return AsOwner();
    }

    Owner& SetStrokeLineJoin(StrokeLineJoin line_join) {
        stroke_linejoin_ = std::move(line_join);
        return AsOwner();
    }

  protected:
    ~PathProps() = default;

    void RenderAttrs(std::ostream& out) const {
        using namespace std::literals;

        if (fill_color_) {
            out << " fill=\""sv << *fill_color_ << "\""sv;
        }
        if (stroke_color_) {
            out << " stroke=\""sv << *stroke_color_ << "\""sv;
        }
        if (stroke_width_) {
            out << " stroke-width=\""sv << *stroke_width_ << "\""sv;
        }

        if (stroke_linecap_) {
            out << " stroke-linecap=\""sv << *stroke_linecap_ << "\""sv;
        }

        if (stroke_linejoin_) {
            out << " stroke-linejoin=\""sv << *stroke_linejoin_ << "\""sv;
        }
    }
  private:
    Owner& AsOwner() {
        // static_cast безопасно преобразует *this к Owner&,
        // если класс Owner — наследник PathProps
        return static_cast<Owner&>(*this);
    }

    std::optional<Color> fill_color_;
    std::optional<Color> stroke_color_;
    std::optional<double> stroke_width_;
    std::optional<StrokeLineCap> stroke_linecap_;
    std::optional<StrokeLineJoin> stroke_linejoin_;
};




struct Point {
    Point() = default;
    Point(double xi, double yi)
        : x(xi)
        , y(yi) {}

    void SetPoint(double x, double y);
    double x = 0.0;
    double y = 0.0;
};

/*
 * Вспомогательная структура, хранящая контекст для вывода SVG-документа с отступами.
 * Хранит ссылку на поток вывода, текущее значение и шаг отступа при выводе элемента
 */
struct RenderContext {
    RenderContext(std::ostream& out)
        : out(out) {
    }

    RenderContext(std::ostream& out, int indent_step, int indent = 0)
        : out(out)
        , indent_step(indent_step)
        , indent(indent) {
    }

    RenderContext Indented() const {
        return {out, indent_step, indent + indent_step};
    }

    void RenderIndent() const {
        for (int i = 0; i < indent; ++i) {
            out.put(' ');
        }
    }

    std::ostream& out;
    int indent_step = 0;
    int indent = 0;
};

/*
 * Абстрактный базовый класс Object служит для унифицированного хранения
 * конкретных тегов SVG-документа
 * Реализует паттерн "Шаблонный метод" для вывода содержимого тега
 */
class Object {
  public:
    virtual void Render(const RenderContext& context) const;

    virtual ~Object() = default;

  private:
    virtual void RenderObject(const RenderContext& context) const = 0;
};

/*
 * Класс Circle моделирует элемент <circle> для отображения круга
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/circle
 */
class Circle final : public Object, public PathProps<Circle> {
  public:
    //Circle(Point p, double r) : center_(p), radius_(r){}
    Circle& SetCenter(Point center);
    Circle& SetRadius(double radius);
    Point GetCenter() const ;
    double GetRadius() const ;
  private:
    void RenderObject(const RenderContext& context) const override;
    Point center_;
    double radius_ = 1.0;
};

/*
 * Класс Polyline моделирует элемент <polyline> для отображения ломаных линий
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/polyline
 */
class Polyline : public Object, public PathProps<Polyline> {
  public:
    // Добавляет очередную вершину к ломаной линии
    Polyline& AddPoint(Point point);
  private:
    void RenderObject(const RenderContext& context) const override;
    std::deque<Point> deq_points;
};

/*
 * Класс Text моделирует элемент <text> для отображения текста
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/text
 */
class Text : public Object, public PathProps<Text> {
  public:
    // Задаёт координаты опорной точки (атрибуты x и y)
    Text& SetPosition(Point pos);

    // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
    Text& SetOffset(Point offset);

    // Задаёт размеры шрифта (атрибут font-size)
    Text& SetFontSize(uint32_t size);

    // Задаёт название шрифта (атрибут font-family)
    Text& SetFontFamily(std::string font_family);

    // Задаёт толщину шрифта (атрибут font-weight)
    Text& SetFontWeight(std::string font_weight);

    // Задаёт текстовое содержимое объекта (отображается внутри тега text)
    Text& SetData(std::string data);
  private:
    void RenderObject(const RenderContext& context) const override;
    std::string text_ = "";
    Point pos_ = {0.0, 0.0};
    Point offset_ = {0.0, 0.0};
    uint32_t size_ = 1;
    std::string font_family_ = "default";
    std::string font_weight_ = "normal";
    const std::set<std::string> default_fonts = {"serif", "sans-serif", "cursive", "fantasy", "monospace"};
};



class ObjectContainer {
  public:
    template<class Obj>
    void Add(Obj obj) {
        std::unique_ptr<Object> my_p_obj = std::make_unique<Obj>(obj);
        svg_objects_.push_back(std::move(my_p_obj));
    }
    void Render(std::ostream& out) const;
    virtual void AddPtr(std::unique_ptr<Object>&& obj) = 0;
  protected:
    std::deque<std::unique_ptr<Object>> svg_objects_;
    ~ObjectContainer() = default;
} ; // namespace svg

class Document : public ObjectContainer {
  public:
    // Добавляет в svg-документ объект-наследник svg::Object
    void AddPtr(std::unique_ptr<Object>&& obj) override;
    // Выводит в ostream svg-представление документа
    // Прочие методы и данные, необходимые для реализации класса Document
};

// Интерфейс Drawable задаёт объекты, которые можно нарисовать с помощью Graphics
class Drawable {
  public:
    virtual void Draw(ObjectContainer& o_c) const = 0;

    virtual ~Drawable() = default;
};
}

namespace shapes {
svg::Polyline CreateStar(svg::Point center, double outer_rad, double inner_rad, int num_rays);


class Triangle : public svg::Drawable {
  public:
    Triangle(svg::Point p1, svg::Point p2, svg::Point p3)
        : p1_(p1)
        , p2_(p2)
        , p3_(p3) {}

    // Реализует метод Draw интерфейса svg::Drawable
    void Draw(svg::ObjectContainer& container) const override ;

  private:
    svg::Point p1_, p2_, p3_;
};

class Star : public svg::Drawable {
  public:
    Star(svg::Point p_s, double o_r, double i_r, int num) : star_poly_(shapes::CreateStar(p_s, o_r, i_r, num)) {
    }

    void Draw(svg::ObjectContainer& container) const override ;

  private:
    svg::Polyline star_poly_;
};

class Snowman : public svg::Drawable {
  public:
    Snowman(svg::Point p_c, double rad) : p_c_(p_c), radius_(rad) {
    }
    void Draw(svg::ObjectContainer& container) const override ;

  private:
    svg::Point p_c_;
    double radius_;
};// namespace shapes
}
