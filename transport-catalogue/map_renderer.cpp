#include "map_renderer.h"

/*
 * В этом файле вы можете разместить код, отвечающий за визуализацию карты маршрутов в формате SVG.
 * Визуализация маршрутов вам понадобится во второй части итогового проекта.
 * Пока можете оставить файл пустым.
 */
bool IsZero(double value) {
    return std::abs(value) < EPSILON;
}

void MapRender::print_route_line() {

    size_t color_counter = 0 ;
    for(const auto bus : buses_) {
        svg::Polyline bus_route;

        bus_route.SetStrokeColor(r_s.color_palette[color_counter % r_s.color_palette.size()]);
        color_counter += 1;

        bus_route.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
        bus_route.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
        bus_route.SetStrokeWidth(r_s.line_width);
        bus_route.SetFillColor(svg::NoneColor);

        if(!bus->its_ring()) {
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

void MapRender::busname_with_ren_setting(Bus* bus, svg::Text &t, svg::Point p){
    t.SetData(bus->name_);
    t.SetPosition(p);
    t.SetOffset(svg::Point(r_s.bus_label_offset[0], r_s.bus_label_offset[1]));
    t.SetFontSize(r_s.bus_label_font_size);
    t.SetFontFamily("Verdana");
    t.SetFontWeight("bold");
}

void MapRender::print_bus_name() {
    size_t color_counter = 0;

    for(const auto bus : buses_) {
        if(bus->route_.empty()) {
            continue;
        }

        svg::Text bus_name;
        busname_with_ren_setting(bus, bus_name, proj({bus->route_[0]->latitude, bus->route_[0]->longitude}));
        bus_name.SetFillColor(r_s.color_palette[color_counter % r_s.color_palette.size()]);
        color_counter += 1;

        svg::Text bus_name_backg = bus_name;
        busname_with_ren_setting(bus, bus_name_backg, proj({bus->route_[0]->latitude, bus->route_[0]->longitude}));
        bus_name_backg.SetStrokeColor(r_s.underlayer_color);
        bus_name_backg.SetStrokeWidth(r_s.underlayer_width);
        bus_name_backg.SetFillColor(r_s.underlayer_color);
        bus_name_backg.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
        bus_name_backg.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

        svg_doc.Add(bus_name_backg);
        svg_doc.Add(bus_name);

        if(!bus->its_ring() && bus->route_[0] != bus->route_[bus->route_.size()-1]) {
            bus_name.SetPosition(proj({bus->route_[bus->route_.size()-1]->latitude, bus->route_[bus->route_.size()-1]->longitude}));
            bus_name_backg.SetPosition(proj({bus->route_[bus->route_.size()-1]->latitude, bus->route_[bus->route_.size()-1]->longitude}));
            svg_doc.Add(bus_name_backg);
            svg_doc.Add(bus_name);
        }
    }
}



void MapRender::print_stop_circle() {
    std::set<Stop*, stop_compare> stops_in_alphabetical_ = stops_in_alphabetical(buses_);
    svg::Color white_color("white");
    double rad = r_s.stop_radius;

    for(const Stop* stop : stops_in_alphabetical_) {
        svg::Circle circle;
        circle.SetFillColor(white_color);
        circle.SetRadius(rad);
        circle.SetCenter(proj({stop->latitude, stop->longitude}));
        svg_doc.Add(circle);
    }
}

void MapRender::stopname_with_ren_setting(Stop* stop, svg::Text &t, svg::Point p) {
    t.SetData(stop->name);
    t.SetPosition(p);
    t.SetOffset(svg::Point(r_s.stop_label_offset[0], r_s.stop_label_offset[1]));
    t.SetFontSize(r_s.stop_label_font_size);
    t.SetFontFamily("Verdana");
}

void MapRender::print_stop_name(){
    std::set<Stop*, stop_compare> stops_in_alphabetical_ = stops_in_alphabetical(buses_);
    svg::Color black_color("black");

    for(Stop* stop : stops_in_alphabetical_) {
        svg::Text stop_name;
        stopname_with_ren_setting(stop, stop_name, proj({stop->latitude, stop->longitude}));
        stop_name.SetFillColor(black_color);

        svg::Text stop_name_backg;
        stopname_with_ren_setting(stop, stop_name_backg, proj({stop->latitude, stop->longitude}));
        stop_name_backg.SetStrokeColor(r_s.underlayer_color);
        stop_name_backg.SetStrokeWidth(r_s.underlayer_width);
        stop_name_backg.SetFillColor(r_s.underlayer_color);
        stop_name_backg.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
        stop_name_backg.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

        svg_doc.Add(stop_name_backg);
        svg_doc.Add(stop_name);
    }

}


SphereProjector MapRender::print_coordinates() {
    std::vector<geo::Coordinates> c_v;
    for(const auto bus : buses_) {
        for(const auto stop : bus->route_) {
            c_v.push_back({stop->latitude, stop->longitude});
        }
    }

    SphereProjector proj{c_v.begin(), c_v.end(), r_s.width, r_s.height, r_s.padding};

    return proj;
}
