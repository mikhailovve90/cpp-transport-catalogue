#include "serialization.h"

void SerializationTC::serialize(){
    std::ofstream out_file(file_for_serialization, std::ios::binary);
    db.SerializeToOstream(&out_file);
}

void SerializationTC::serialize_tr(TransportRouter& t_r) {
    db.mutable_route_settings()->set_wait_time(t_r.get_wait_time());
    db.mutable_route_settings()->set_average_bus_speed(t_r.get_bus_speed());
   /* for(size_t r = 0; r < t_r.get_edge_count(); ++r){
        tc_serialize::Edge edge;
        auto edge_from_graph = t_r.get_edge(r);
        edge.set_vertex_id_from(edge_from_graph.from);
        edge.set_vertex_id_to(edge_from_graph.to);
        edge.set_weight(edge_from_graph.weight);
        edge.set_bus_name(t_r.get_edge_content_from_edge_id(r).first->name_);
        *db.add_edges() = edge;
    }*/
}

void SerializationTC::deserialize_tr(TransportRouter& t_r) {
    t_r.set_bus_speed(db.mutable_route_settings()->average_bus_speed());
    t_r.set_wait_time(db.mutable_route_settings()->wait_time());
}

void SerializationTC::serialize_tc(TransportCatalogue& t_c) {
      std::map<const Stop*, int> stop_to_id;
      const std::deque<Stop>& stops =  t_c.get_stops();

      for(size_t i = 0; i < stops.size(); ++i) {
          tc_serialize::Stop stop;
          stop.set_name(stops.at(i).name_);
          stop.set_id(i);
          stop_to_id[&stops.at(i)] = i;
          tc_serialize::Coordinates coordinates;
          coordinates.set_latitude(stops[i].latitude);
          coordinates.set_longitude(stops[i].longitude);
          *stop.mutable_coordinate() = coordinates;
          *db.add_stops() = stop;
           tc_serialize::Distance dist;
           for(size_t s = 0; s < stops.size(); ++s) {
              long our_dist = t_c.get_distance_between_stops(stops.at(i).name_, stops.at(s).name_);
              if(our_dist > 0) {
                  dist.set_id1(i);
                  dist.set_id2(s);
                  dist.set_distance(our_dist);
                  *db.add_distances() = dist;
              }
              our_dist = t_c.get_distance_between_stops(stops.at(s).name_, stops.at(i).name_);
              if(our_dist > 0) {
                  dist.set_id1(s);
                  dist.set_id2(i);
                  dist.set_distance(our_dist);
                  *db.add_distances() = dist;
              }
          }
      }

      const std::deque<Bus>& buses = t_c.get_buses();
      for(size_t i = 0; i < buses.size(); ++i) {
          tc_serialize::Bus bus;
          bus.set_name(buses[i].name_);
          if(buses[i].is_circular_route()) {
              bus.set_start(stop_to_id[buses[i].route_[0]]);
              bus.set_stop(stop_to_id[buses[i].route_[0]]);
          } else {
              bus.set_start(stop_to_id[buses[i].route_[0]]);
              bus.set_stop(stop_to_id[buses[i].route_[buses[i].route_.size() -1]]);
          }
          size_t route_size = buses[i].is_circular_route() ? buses[i].route_.size() : buses[i].route_.size();
          for(size_t t = 0; t < route_size; ++t) {
              bus.add_route(stop_to_id[buses[i].route_[t]]);
          }
          bus.set_is_circ(buses[i].is_circular_route());
          *db.add_buses() = bus;
      }
}
void SerializationTC::deserialize() {
    std::ifstream in_file(file_for_serialization, std::ios::binary);
    db.ParseFromIstream(&in_file);
}


TransportCatalogue SerializationTC::deserialize_tc() {
    TransportCatalogue result;
    std::map<int, std::string> id_to_stop;
    for(size_t h = 0; h < db.stops_size(); ++h) {
          result.add_stop(db.mutable_stops(h)->name(),
                          db.mutable_stops(h)->mutable_coordinate()->latitude(),
                          db.mutable_stops(h)->mutable_coordinate()->longitude());
          id_to_stop[db.mutable_stops(h)->id()] = db.mutable_stops(h)->name();
    }

    for(size_t f = 0; f < db.buses_size(); ++f) {
        bool is_ring = db.mutable_buses(f)->is_circ();
        std::vector<Stop*> stops_on_route;
        for(int p = 0; p < db.mutable_buses(f)->route_size(); ++p){
              Stop* stop = result.pointer_stop_name(id_to_stop[db.mutable_buses(f)->route(p)]);
              stops_on_route.push_back(stop);
        }
        result.add_bus(db.mutable_buses(f)->name(), stops_on_route, is_ring);
    }

    for(size_t r = 0; r < db.distances_size(); ++r) {
         result.add_distance_to_stop(id_to_stop[db.mutable_distances(r)->id1()],
                                  id_to_stop[db.mutable_distances(r)->id2()],
                                  db.mutable_distances(r)->distance());
    }

    return result;
}




void message_color_from_svg_color(tc_serialize::Color& ms_col, const svg::Color& svg_col) {
    size_t index_underlayer_color = svg_col.index();
    switch (index_underlayer_color) {
         case 1: {
            std::string str_color = std::get<1>(svg_col);
            ms_col.mutable_name()->set_col_name(str_color);
            break;
         }
         case 2: {
            svg::Rgb rgb_color = std::get<2>(svg_col);
            ms_col.mutable_rgb()->set_red(rgb_color.red);
            ms_col.mutable_rgb()->set_green(rgb_color.green);
            ms_col.mutable_rgb()->set_blue(rgb_color.blue);
            break;
         }
         case 3: {
            svg::Rgba rgba_color = std::get<3>(svg_col);
            ms_col.mutable_rgb()->set_red(rgba_color.red);
            ms_col.mutable_rgb()->set_green(rgba_color.green);
            ms_col.mutable_rgb()->set_blue(rgba_color.blue);
            ms_col.mutable_rgb()->mutable_opacity()->set_num(rgba_color.opacity);
            break;
         }
         default:
         break;
      }
}

void SerializationTC::serialize_rs(RenderSettings& r_s) {
    db.mutable_render_settings()->set_width(r_s.width);
    db.mutable_render_settings()->set_height(r_s.height);
    db.mutable_render_settings()->set_padding(r_s.padding);
    db.mutable_render_settings()->set_line_width(r_s.line_width);
    db.mutable_render_settings()->set_stop_radius(r_s.stop_radius);
    db.mutable_render_settings()->set_bus_label_font_size(r_s.bus_label_font_size);
    for(size_t y = 0; y < r_s.bus_label_offset.size(); ++y) {
        db.mutable_render_settings()->add_bus_label_offset(r_s.bus_label_offset[y]);
    }
    db.mutable_render_settings()->set_stop_label_font_size(r_s.stop_label_font_size);
    for(size_t y = 0; y < r_s.stop_label_offset.size(); ++y) {
        db.mutable_render_settings()->add_stop_label_offset(r_s.stop_label_offset[y]);
    }

    tc_serialize::Color underlayer_color;
    message_color_from_svg_color(underlayer_color, r_s.underlayer_color);
    *db.mutable_render_settings()->mutable_underlayer_color() = underlayer_color;

    db.mutable_render_settings()->set_underlayer_width(r_s.underlayer_width);

    for(size_t y = 0; y < r_s.color_palette.size(); ++y) {
        tc_serialize::Color add_color;
        message_color_from_svg_color(add_color, r_s.color_palette[y]);
        *db.mutable_render_settings()->add_color_palette() = add_color;
    }
}

void svg_color_from_message_color(svg::Color& svg_col , const tc_serialize::Color& ms_col) {
    if(ms_col.has_rgb()) {
        if(ms_col.rgb().has_opacity()) {
              svg::Rgba rgba(static_cast<short>(ms_col.rgb().red()),
                             static_cast<short>(ms_col.rgb().green()),
                             static_cast<short>(ms_col.rgb().blue()),
                             ms_col.rgb().opacity().num());
          svg_col = rgba;
        } else {
            svg::Rgb rgb(static_cast<short>(ms_col.rgb().red()),
                             static_cast<short>(ms_col.rgb().green()),
                             static_cast<short>(ms_col.rgb().blue()));
            svg_col = rgb;
        }
    } else if(ms_col.has_name()){
        svg_col = ms_col.name().col_name();
    }

}


RenderSettings SerializationTC::deserialize_rs() {
    RenderSettings result;
    result.width = db.mutable_render_settings()->width();
    result.height = db.mutable_render_settings()->height();
    result.padding = db.mutable_render_settings()->padding();
    result.line_width = db.mutable_render_settings()->line_width();
    result.stop_radius = db.mutable_render_settings()->stop_radius();
    result.bus_label_font_size = db.mutable_render_settings()->bus_label_font_size();

    result.bus_label_offset.resize(db.mutable_render_settings()->bus_label_offset_size());
    for(size_t r = 0; r < result.bus_label_offset.size(); ++r) {
        result.bus_label_offset[r] = db.mutable_render_settings()->bus_label_offset(r);
    }

    result.stop_label_font_size = db.mutable_render_settings()->stop_label_font_size();

    result.stop_label_offset.resize(db.mutable_render_settings()->stop_label_offset_size());
    for(size_t r = 0; r < result.stop_label_offset.size(); ++r) {
        result.stop_label_offset[r] = db.mutable_render_settings()->stop_label_offset(r);
    }

    svg::Color underlayer_color;
    svg_color_from_message_color(underlayer_color, *db.mutable_render_settings()->mutable_underlayer_color());
    result.underlayer_color = underlayer_color;
    result.underlayer_width = db.mutable_render_settings()->underlayer_width();


    result.color_palette.resize(db.mutable_render_settings()->color_palette_size());
    for(size_t j = 0; j < result.color_palette.size(); ++j) {
        svg::Color col;
        svg_color_from_message_color(col, db.mutable_render_settings()->color_palette(j));
        result.color_palette[j] = col;
    }

    return result;
}



void SerializationTC::set_file_serialization_name(const std::string& name) {
    file_for_serialization = name;
}

const std::string& SerializationTC::get_file_serialization() {
    return file_for_serialization;
}
