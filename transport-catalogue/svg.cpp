#include "svg.h"

namespace svg {

void PrintColor(std::ostream& out, std::monostate)  {
    out << "none";
}

void PrintColor(std::ostream& out, std::string name) {
    out << name;
}

void PrintColor(std::ostream& out, svg::Rgb rgb)  {
    out << "rgb(" << static_cast<short>(rgb.red) << "," << static_cast<short>(rgb.green) << "," << static_cast<short>(rgb.blue) << ")";
}

void PrintColor(std::ostream& out, svg::Rgba& rgba) {
    out << "rgba(" << static_cast<short>(rgba.red) << "," << static_cast<short>(rgba.green) << "," << static_cast<short>(rgba.blue) << "," << rgba.opacity << ")";
}

std::ostream& operator<<(std::ostream &os, const Color &col) {
    visit(
    [&os](auto value) {
        // Это универсальная лямбда-функция (generic lambda).
        // Внутри неё нужная функция PrintRoots будет выбрана за счёт перегрузки функций.
        PrintColor(os, value);
    }, col);
    return os;
}

std::ostream& operator<<(std::ostream &os, const std::optional<Color> &col) {
    visit(
    [&os](auto value) {
        // Это универсальная лямбда-функция (generic lambda).
        // Внутри неё нужная функция PrintRoots будет выбрана за счёт перегрузки функций.
        PrintColor(os, value);
    }, col.value());
    return os;
}


std::ostream& operator<<(std::ostream &os, const StrokeLineCap &stcp) {
    std::deque<std::string> linecap_name = {"butt","round","square"};
    return os << linecap_name[static_cast<size_t>(stcp)];
}

std::ostream& operator<<(std::ostream &os, const StrokeLineJoin &stlj) {
    std::deque<std::string> linejoin_name = {"arcs","bevel","miter", "miter-clip", "round"};
    return os << linejoin_name[static_cast<size_t>(stlj)];
}



std::string& text_to_svg_format(std::string& text) {
    std::deque<std::pair<char, std::string>> dictionary = {
        std::make_pair('&', "&amp;"),
        std::make_pair('"', "&quot;"),
        std::make_pair('\'', "&apos;"),
        std::make_pair('<', "&lt;"),
        std::make_pair('>', "&gt;"),
    };
    for(const auto pair : dictionary) {
        auto pos = text.find(pair.first);
        while(pos != std::string::npos) {
            text.replace(pos, 1, pair.second);
            pos = text.find(pair.first, pos + pair.second.size());
        }
    }
    return text;
}

using namespace std::literals;

void Point::SetPoint(double xi, double yi) {
    x = xi;
    y = yi;
}

void Object::Render(const RenderContext& context) const {
    context.RenderIndent();

    // Делегируем вывод тега своим подклассам
    RenderObject(context);

    context.out << std::endl;
}
//----------------------------------------------Path--------------------------------------------








// ---------- Circle ------------------

Circle& Circle::SetCenter(Point center)  {
    center_ = center;
    return *this;
}

Circle& Circle::SetRadius(double radius)  {
    radius_ = radius;
    return *this;
}

Point Circle::GetCenter() const {
    return center_;
}

double Circle::GetRadius() const {
    return radius_;
}

void Circle::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
    out << "r=\""sv << radius_ << "\" "sv;
    RenderAttrs(context.out);
    out << "/>"sv;
}

// ---------------- Polyline ---------------------
Polyline& Polyline::AddPoint(Point point) {
    deq_points.push_back(point);
    return *this;
}

void Polyline::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<polyline points=\""sv ;
    for(size_t q  = 0; q < deq_points.size(); ++q) {
        if(q != deq_points.size() - 1) {
            out << deq_points[q].x << "," << deq_points[q].y << " ";
            continue;
        }
        out << deq_points[q].x << "," << deq_points[q].y;
    }

    out << "\""sv;
    RenderAttrs(out);
    out << "/>"sv;
}

// ---------------------- Text -------------------------------
Text& Text::SetPosition(Point pos) {
    pos_ = pos;
    return *this;
}

// Задаёт смещение относительно опорной точки (атрибуты dx, dy)
Text& Text::SetOffset(Point offset) {
    offset_ = offset;
    return *this;
}

// Задаёт размеры шрифта (атрибут font-size)
Text& Text::SetFontSize(uint32_t size) {
    size_ = size;
    return *this;
}

// Задаёт название шрифта (атрибут font-family)
Text& Text::SetFontFamily(std::string font_family) {
    font_family_ = font_family;
    return *this;
}

// Задаёт толщину шрифта (атрибут font-weight)
Text& Text::SetFontWeight(std::string font_weight) {
    font_weight_ = font_weight;
    return *this;
}

// Задаёт текстовое содержимое объекта (отображается внутри тега text)
Text& Text::SetData(std::string data) {
    text_ = text_to_svg_format(data);
    return *this;
}

void Text::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<text x=\""sv << pos_.x << "\" y=\""sv << pos_.y << "\" "sv;
    out << "dx=\""sv << offset_.x << "\" dy=\""sv << offset_.y<< "\""sv;
    out << " font-size=\""sv << size_ << "\""sv;

    if(font_family_ != "default") {
        out << " font-family=\""sv << font_family_ << "\""sv;
    }
    if(font_weight_ != "normal") {
        out << " font-weight=\""sv << font_weight_ << "\""sv;
    }
    RenderAttrs(out);
    out << ">"sv << text_ << "</text>";
}


// ----------------------------------- Documents --------------------------------------

// Добавляет в svg-документ объект-наследник svg::Object
void Document::AddPtr(std::unique_ptr<Object>&& obj) {
    this->svg_objects_.push_back(std::move(obj));
}

// Выводит в ostream svg-представление документа
void ObjectContainer::Render(std::ostream& out) const {
    RenderContext ren(out, 2, 2);
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << std::endl;
    out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv << std::endl;
    for(size_t i = 0; i < svg_objects_.size(); ++i) {
        svg_objects_[i].get()->Render(ren);
    }
    out << "</svg>"sv;
}
}// namespace svg
namespace shapes {
//--------------------------------------- Star -----------------------------------
svg::Polyline CreateStar(svg::Point center, double outer_rad, double inner_rad, int num_rays) {
    using namespace svg;
    Polyline polyline;
    for (int i = 0; i <= num_rays; ++i) {
        double angle = 2 * M_PI * (i % num_rays) / num_rays;
        polyline.AddPoint({center.x + outer_rad * sin(angle), center.y - outer_rad * cos(angle)});
        if (i == num_rays) {
            break;
        }
        angle += M_PI / num_rays;
        polyline.AddPoint({center.x + inner_rad * sin(angle), center.y - inner_rad * cos(angle)});
    }
    return polyline;
}

void Star::Draw(svg::ObjectContainer& container) const {
    svg::Polyline star = star_poly_;
    container.Add(star.SetFillColor("red").SetStrokeColor("black"));
}

// -------------------------------------Snowman ---------------------------------
void Snowman::Draw(svg::ObjectContainer& container) const {
    container.Add(svg::Circle().SetCenter({p_c_.x, p_c_.y + 5 * radius_}).SetRadius(2 * radius_).SetFillColor("rgb(240,240,240)").SetStrokeColor("black"));
    container.Add(svg::Circle().SetCenter({p_c_.x, p_c_.y + 2 * radius_}).SetRadius(1.5 * radius_).SetFillColor("rgb(240,240,240)").SetStrokeColor("black"));
    container.Add(svg::Circle().SetCenter(p_c_).SetRadius(radius_).SetFillColor("rgb(240,240,240)").SetStrokeColor("black"));
}

//-------------------------------Triangle----------------------------------------------
void Triangle::Draw(svg::ObjectContainer& container) const {
    container.Add(svg::Polyline().AddPoint(p1_).AddPoint(p2_).AddPoint(p3_).AddPoint(p1_));
}
}
