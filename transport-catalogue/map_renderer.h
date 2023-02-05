#pragma once
#include "svg.h"
#include  <vector>
#include "geo.h"
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <optional>
#include "domain.h"
#include "transport_catalogue.h"
/*
 * В этом файле вы можете разместить код, отвечающий за визуализацию карты маршрутов в формате SVG.
 * Визуализация маршрутов вам понадобится во второй части итогового проекта.
 * Пока можете оставить файл пустым.
 */
struct RenderSettings {
    double width;
    double height;
    double padding;

    double line_width;
    double stop_radius;

    int bus_label_font_size;
    std::deque<double> bus_label_offset;

    int stop_label_font_size;
    std::deque<double>stop_label_offset;

    svg::Color underlayer_color;
    double underlayer_width;

    std::deque<svg::Color> color_palette;
};

inline const double EPSILON = 1e-6;
bool IsZero(double value);

class SphereProjector {
  public:
    // points_begin и points_end задают начало и конец интервала элементов geo::Coordinates
    template <typename PointInputIt>
    SphereProjector(PointInputIt points_begin, PointInputIt points_end,
                    double max_width, double max_height, double padding)
        : padding_(padding) { //
        // Если точки поверхности сферы не заданы, вычислять нечего
        if (points_begin == points_end) {
            return;
        }

        // Находим точки с минимальной и максимальной долготой
        const auto [left_it, right_it] = std::minmax_element(
                                             points_begin, points_end,
        [](auto lhs, auto rhs) {
            return lhs.lng < rhs.lng;
        });
        min_lon_ = left_it->lng;
        const double max_lon = right_it->lng;

        // Находим точки с минимальной и максимальной широтой
        const auto [bottom_it, top_it] = std::minmax_element(
                                             points_begin, points_end,
        [](auto lhs, auto rhs) {
            return lhs.lat < rhs.lat;
        });
        const double min_lat = bottom_it->lat;
        max_lat_ = top_it->lat;

        // Вычисляем коэффициент масштабирования вдоль координаты x
        std::optional<double> width_zoom;
        if (!IsZero(max_lon - min_lon_)) {
            width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
        }

        // Вычисляем коэффициент масштабирования вдоль координаты y
        std::optional<double> height_zoom;
        if (!IsZero(max_lat_ - min_lat)) {
            height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
        }

        if (width_zoom && height_zoom) {
            // Коэффициенты масштабирования по ширине и высоте ненулевые,
            // берём минимальный из них
            zoom_coeff_ = std::min(*width_zoom, *height_zoom);
        } else if (width_zoom) {
            // Коэффициент масштабирования по ширине ненулевой, используем его
            zoom_coeff_ = *width_zoom;
        } else if (height_zoom) {
            // Коэффициент масштабирования по высоте ненулевой, используем его
            zoom_coeff_ = *height_zoom;
        }
    }

    // Проецирует широту и долготу в координаты внутри SVG-изображения
    svg::Point operator()(geo::Coordinates coords) const {
        return {
            (coords.lng - min_lon_) * zoom_coeff_ + padding_,
            (max_lat_ - coords.lat) * zoom_coeff_ + padding_
        };
    }
  private:
    double padding_;
    double min_lon_ = 0;
    double max_lat_ = 0;
    double zoom_coeff_ = 0;
};

class MapRenderer {
  public:
    MapRenderer(const TransportCatalogue& t_c, const RenderSettings& rs):
        transport_catalogue_(t_c), buses_(t_c.all_buses()), render_settings(rs), proj(render_coordinates()) {};
    void render_route_line();
    void render_bus_name();
    void render_stop_circle();
    void render_stop_name();
    svg::Document& get_svg_doc() {
        return svg_doc;
    }

    void busname_with_ren_setting(Bus* bus, svg::Text &t, svg::Point p);
    void stopname_with_ren_setting(Stop* stop, svg::Text &t, svg::Point p);
    SphereProjector render_coordinates();
  private:
    const TransportCatalogue& transport_catalogue_;
    svg::Document svg_doc;
    const std::set<Bus*, bus_compare>& buses_;
    const RenderSettings& render_settings;
    SphereProjector proj;
};
