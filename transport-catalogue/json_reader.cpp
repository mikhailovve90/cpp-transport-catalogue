#include "json_reader.h"

/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */
json::Document read_json(std::istream& input_stream) {
    return json::Load(input_stream);
}

JsonProcessing& JsonProcessing::json_to_tc(TransportCatalogue& t_c) {
    std::vector<json::Node> base_req = doc_being_proc.GetRoot().AsMap().at("base_requests").AsArray();
    std::unordered_map<std::string, std::deque<std::pair<std::string, int>>> dist_map;

    for(const json::Node& n_d : base_req) {
        auto& map_node = n_d.AsMap();
        if(map_node.count("type")) {
            if(map_node.at("type").AsString() == "Stop") {
                t_c.add_stop(map_node.at("name").AsString(), map_node.at("latitude").AsDouble(), map_node.at("longitude").AsDouble());
                for(const auto[stop_name, dist] : map_node.at("road_distances").AsMap()) {
                    dist_map[map_node.at("name").AsString()].push_back(std::pair(stop_name, dist.AsInt()));
                }
            }
        }
    }

    for(const auto[stop_name, deq_st_dist] : dist_map) {
        for(const auto& p : deq_st_dist) {
            t_c.add_distance_to_stop(stop_name, p.first,  p.second);
        }
    }

    for(const json::Node& n_d : base_req) {
        std::vector<Stop*> route;
        auto& map_node = n_d.AsMap();
        if(map_node.count("type")) {
            if(map_node.at("type").AsString() == "Bus") {
                for(const json::Node stop : map_node.at("stops").AsArray()) {
                    route.push_back(t_c.pointer_stop_name(stop.AsString()));
                }
                if(map_node.at("is_roundtrip").AsBool()) {
                    route.push_back(t_c.pointer_stop_name(map_node.at("stops").AsArray()[0].AsString()));
                }
                t_c.add_bus(map_node.at("name").AsString(), route, map_node.at("is_roundtrip").AsBool());
            }
        }
    }
    return *this;
}

svg::Color JsonProcessing::set_rgb_rgba_color(const std::vector<json::Node>& c_array) {
    if(c_array.size() == 3) {
        return svg::Color(svg::Rgb(c_array[0].AsInt(),
                                              c_array[1].AsInt(),
                                              c_array[2].AsInt()));
    } else if (c_array.size() == 4) {
        return svg::Color(svg::Rgba(c_array[0].AsInt(),
                                           c_array[1].AsInt(),
                                           c_array[2].AsInt(),
                                           c_array[3].AsDouble()));
    }

    return svg::NoneColor;
}

void JsonProcessing::set_color_pallete(const std::vector<json::Node>& c_p_array, RenderSettings& r_s) {
    for(size_t s_t = 0; s_t < c_p_array.size(); s_t++) {
        if(c_p_array.at(s_t).IsString()) {
            r_s.color_palette.push_back(svg::Color(c_p_array.at(s_t).AsString()));
            continue;
        } else if(c_p_array.at(s_t).IsArray()) {
            r_s.color_palette.push_back(set_rgb_rgba_color(c_p_array.at(s_t).AsArray()));
            continue;
        }
    }
}


JsonProcessing& JsonProcessing::json_to_rs(RenderSettings& r_s) {
    json::Dict ren_set = doc_being_proc.GetRoot().AsMap().at("render_settings").AsMap();
    r_s.width = ren_set["width"].AsDouble();
    r_s.height = ren_set["height"].AsDouble();

    if(ren_set["padding"].AsDouble() < 0) {
        r_s.padding = 0;
    } else {
        r_s.padding = ren_set["padding"].AsDouble();
    }

    r_s.line_width = ren_set["line_width"].AsDouble();
    r_s.stop_radius = ren_set["stop_radius"].AsDouble();

    r_s.bus_label_font_size = ren_set["bus_label_font_size"].AsDouble();
    for(size_t s_t = 0; s_t < ren_set["bus_label_offset"].AsArray().size(); s_t++) {
        r_s.bus_label_offset.push_back(ren_set["bus_label_offset"].AsArray()[s_t].AsDouble());
    }

    r_s.stop_label_font_size = ren_set["stop_label_font_size"].AsInt();
    for(size_t s_t = 0; s_t < ren_set["stop_label_offset"].AsArray().size(); s_t++) {
        r_s.stop_label_offset.push_back(ren_set["stop_label_offset"].AsArray()[s_t].AsDouble());
    }

    set_color_pallete(ren_set["color_palette"].AsArray(), r_s);

    for(size_t s_t = 0; s_t < ren_set["stop_label_offset"].AsArray().size(); s_t++) {
        r_s.bus_label_offset.push_back(ren_set["stop_label_offset"].AsArray()[s_t].AsDouble());
    }

    r_s.underlayer_width = ren_set["underlayer_width"].AsDouble();

    if(ren_set["underlayer_color"].IsString()) {
        r_s.underlayer_color = svg::Color(ren_set["underlayer_color"].AsString());
    } else if (ren_set["underlayer_color"].IsArray() ) {
        r_s.underlayer_color  = set_rgb_rgba_color(ren_set["underlayer_color"].AsArray());
    }
    return *this;
}


json::Dict JsonProcessing::error_dict(int id) {
    return {{"request_id", id},
        {"error_message", json::Node(err_msg)}};
}

std::string& JsonProcessing::svg_to_json_format(std::string& text) {
    std::deque<std::pair<char, std::string>> dictionary = {
        std::make_pair('\\', "\\"),
        std::make_pair('"' , "\""),
        std::make_pair('\r', "\r"),
        std::make_pair('\n', "\n"),
    };
    for(const auto pair : dictionary) {
        auto pos = text.find(pair.first);
        while(pos != std::string::npos) {
            text.replace(pos, 1, pair.second);
            pos = text.find(pair.first , pos + pair.second.size());
        }
    }
      return text;
}

std::vector<json::Node> JsonProcessing::json_stat_from_tc(RequestHandler& r_h) {
    std::vector<json::Node> stat_requests = doc_being_proc.GetRoot().AsMap().at("stat_requests").AsArray();
    std::vector<json::Node> result;

    for(const json::Node& n_d : stat_requests) {
        auto& map_node = n_d.AsMap();
        if(map_node.at("type").AsString() == "Stop") {
            if(r_h.get_ref_to_t_c().pointer_stop_name(map_node.at("name").AsString()) == nullptr) {
                result.push_back(error_dict(map_node.at("id").AsInt()));
            } else {
                const std::set<Bus*, bus_compare>& buses = r_h.get_ref_to_t_c().stopname_to_buses(map_node.at("name").AsString());
                std::vector<json::Node> buses_name;
                for(const auto bus : buses) {
                    buses_name.push_back(json::Node(bus->name_));
                }

                json::Dict good_req = {{"request_id", json::Node(map_node.at("id").AsInt())},
                    {"buses", json::Node(buses_name)}
                };
                result.push_back(good_req);
            }
        } else if(map_node.at("type").AsString() == "Bus") {
            if(r_h.get_ref_to_t_c().pointer_bus_name(map_node.at("name").AsString()) == nullptr) {
                result.push_back(error_dict(map_node.at("id").AsInt()));
            } else {
                const auto a = map_node.at("name").AsString();
                BusInfo b_i = r_h.get_bus_info(r_h.get_ref_to_t_c().pointer_bus_name(map_node.at("name").AsString()));

                json::Dict good_req = {{"request_id", json::Node(map_node.at("id").AsInt())},
                    {"curvature", b_i.curvature_},
                    {"route_length", b_i.route_length_},
                    {"stop_count", b_i.stop_count_},
                    {"unique_stop_count", b_i.unique_stop_count_}
                };

                result.push_back(good_req);
            }
        } else if(map_node.at("type").AsString() == "Map") {
            svg::Document svg_doc;
            std::ostringstream out;
            r_h.print_map(out);
            std::string svg_str = out.str();
            svg_to_json_format(svg_str);

            json::Dict svg_dict = {{"request_id", json::Node(map_node.at("id").AsInt())}, {"map" , json::Node(svg_str)}};

            result.push_back(svg_dict);
        }
    }
    return result;
}



