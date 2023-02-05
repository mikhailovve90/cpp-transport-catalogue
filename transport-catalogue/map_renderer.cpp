#include "map_renderer.h"

/*
 * В этом файле вы можете разместить код, отвечающий за визуализацию карты маршрутов в формате SVG.
 * Визуализация маршрутов вам понадобится во второй части итогового проекта.
 * Пока можете оставить файл пустым.
 */
bool IsZero(double value) {
    return std::abs(value) < EPSILON;
}

void MapRenderer::render_route_line() {

    size_t color_counter = 0 ;
    for(const auto bus : buses_) {
        svg::Polyline bus_route;

        bus_route.SetStrokeColor(render_settings.color_palette[color_counter % render_settings.color_palette.size()]);
        color_counter += 1;

        bus_route.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
        bus_route.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
        bus_route.SetStrokeWidth(render_settings.line_width);
        bus_route.SetFillColor(svg::NoneColor);

        if(!bus->is_ring()) {
            size_t t = 0;
            for(; t < bus->route_.size(); ++t) {
                svg::Point point = proj({bus->route_[t]->latitude, bus->route_[t]->longitude});
                bus_route.AddPoint(point);
            }

            if(t >= 2) {
                t -= 2;
            }

            for(; t != 0 ; --t) {
                svg::Point point = proj({bus->route_[t]->latitude, bus->route_[t]->longitude});
                bus_route.AddPoint(point);
            }
            svg::Point point = proj({bus->route_[t]->latitude, bus->route_[t]->longitude});
            bus_route.AddPoint(point);
        } else {
            size_t t = 0;
            for(; t < bus->route_.size() - 1; ++t) {
                svg::Point point = proj({bus->route_[t]->latitude, bus->route_[t]->longitude});
                bus_route.AddPoint(point);
            }

        }
        svg_doc.Add(bus_route);
    }
}

void MapRenderer::busname_with_ren_setting(Bus* bus, svg::Text &t, svg::Point p) {
    t.SetData(bus->name_);
    t.SetPosition(p);
    t.SetOffset(svg::Point(render_settings.bus_label_offset[0], render_settings.bus_label_offset[1]));
    t.SetFontSize(render_settings.bus_label_font_size);
    t.SetFontFamily("Verdana");
    t.SetFontWeight("bold");
}

void MapRenderer::render_bus_name() {
    size_t color_counter = 0;

    for(const auto bus : buses_) {
        if(bus->route_.empty()) {
            continue;
        }

        svg::Text bus_name;
        busname_with_ren_setting(bus, bus_name, proj({bus->route_[0]->latitude, bus->route_[0]->longitude}));
        bus_name.SetFillColor(render_settings.color_palette[color_counter % render_settings.color_palette.size()]);
        color_counter += 1;

        svg::Text bus_name_backg = bus_name;
        busname_with_ren_setting(bus, bus_name_backg, proj({bus->route_[0]->latitude, bus->route_[0]->longitude}));
        bus_name_backg.SetStrokeColor(render_settings.underlayer_color);
        bus_name_backg.SetStrokeWidth(render_settings.underlayer_width);
        bus_name_backg.SetFillColor(render_settings.underlayer_color);
        bus_name_backg.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
        bus_name_backg.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

        svg_doc.Add(bus_name_backg);
        svg_doc.Add(bus_name);

        if(!bus->is_ring() && bus->route_[0] != bus->route_[bus->route_.size()-1]) {
            bus_name.SetPosition(proj({bus->route_[bus->route_.size()-1]->latitude, bus->route_[bus->route_.size()-1]->longitude}));
            bus_name_backg.SetPosition(proj({bus->route_[bus->route_.size()-1]->latitude, bus->route_[bus->route_.size()-1]->longitude}));
            svg_doc.Add(bus_name_backg);
            svg_doc.Add(bus_name);
        }
    }
}



void MapRenderer::render_stop_circle() {
    std::set<Stop*, stop_compare> stops_in_alphabetical_ = t_c_.stops_in_alphabetical(buses_);
    svg::Color white_color("white");
    double rad = render_settings.stop_radius;

    for(const Stop* stop : stops_in_alphabetical_) {
        svg::Circle circle;
        circle.SetFillColor(white_color);
        circle.SetRadius(rad);
        circle.SetCenter(proj({stop->latitude, stop->longitude}));
        svg_doc.Add(circle);
    }
}

void MapRenderer::stopname_with_ren_setting(Stop* stop, svg::Text &t, svg::Point p) {
    t.SetData(stop->name_);
    t.SetPosition(p);
    t.SetOffset(svg::Point(render_settings.stop_label_offset[0], render_settings.stop_label_offset[1]));
    t.SetFontSize(render_settings.stop_label_font_size);
    t.SetFontFamily("Verdana");
}

void MapRenderer::render_stop_name() {
    std::set<Stop*, stop_compare> stops_in_alphabetical_ = t_c_.stops_in_alphabetical(buses_);
    svg::Color black_color("black");

    for(Stop* stop : stops_in_alphabetical_) {
        svg::Text stop_name;
        stopname_with_ren_setting(stop, stop_name, proj({stop->latitude, stop->longitude}));
        stop_name.SetFillColor(black_color);

        svg::Text stop_name_backg;
        stopname_with_ren_setting(stop, stop_name_backg, proj({stop->latitude, stop->longitude}));
        stop_name_backg.SetStrokeColor(render_settings.underlayer_color);
        stop_name_backg.SetStrokeWidth(render_settings.underlayer_width);
        stop_name_backg.SetFillColor(render_settings.underlayer_color);
        stop_name_backg.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
        stop_name_backg.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

        svg_doc.Add(stop_name_backg);
        svg_doc.Add(stop_name);
    }

}


SphereProjector MapRenderer::render_coordinates() {
    std::vector<geo::Coordinates> c_v;
    for(const auto bus : buses_) {
        for(const auto stop : bus->route_) {
            c_v.push_back({stop->latitude, stop->longitude});
        }
    }

    SphereProjector proj{c_v.begin(), c_v.end(), render_settings.width, render_settings.height, render_settings.padding};

    return proj;
}
