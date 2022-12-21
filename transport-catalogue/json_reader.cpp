#include "json_reader.h"

/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */
json::Document json_reader::JSONReader::read_json(std::istream& input_stream) {
    return json::Load(input_stream);
}

json_reader::JSONReader& json_reader::JSONReader::parse_transport_catalogue(TransportCatalogue& t_c) {
    std::vector<json::Node> base_req = doc_being_proc.get_root().as_map().at("base_requests").as_array();
    std::unordered_map<std::string, std::deque<std::pair<std::string, int>>> dist_map;

    for(const json::Node& n_d : base_req) {
        auto& map_node = n_d.as_map();
        if(map_node.count("type")) {
            if(map_node.at("type").as_string() == "Stop") {
                t_c.add_stop(map_node.at("name").as_string(), map_node.at("latitude").as_double(), map_node.at("longitude").as_double());
                for(const auto[stop_name, dist] : map_node.at("road_distances").as_map()) {
                    dist_map[map_node.at("name").as_string()].push_back(std::pair(stop_name, dist.as_int()));
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
        auto& map_node = n_d.as_map();
        if(map_node.count("type")) {
            if(map_node.at("type").as_string() == "Bus") {
                for(const json::Node stop : map_node.at("stops").as_array()) {
                    route.push_back(t_c.pointer_stop_name(stop.as_string()));
                }
                if(map_node.at("is_roundtrip").as_bool()) {
                    route.push_back(t_c.pointer_stop_name(map_node.at("stops").as_array()[0].as_string()));
                }
                t_c.add_bus(map_node.at("name").as_string(), route, map_node.at("is_roundtrip").as_bool());
            }
        }
    }
    return *this;
}

svg::Color json_reader::JSONReader::set_rgb_rgba_color(const std::vector<json::Node>& c_array) {
    if(c_array.size() == 3) {
        return svg::Color(svg::Rgb(c_array[0].as_int(),
                                              c_array[1].as_int(),
                                              c_array[2].as_int()));
    } else if (c_array.size() == 4) {
        return svg::Color(svg::Rgba(c_array[0].as_int(),
                                           c_array[1].as_int(),
                                           c_array[2].as_int(),
                                           c_array[3].as_double()));
    }

    return svg::NoneColor;
}

void json_reader::JSONReader::set_color_pallete(const std::vector<json::Node>& c_p_array, RenderSettings& r_s) {
    for(size_t s_t = 0; s_t < c_p_array.size(); s_t++) {
        if(c_p_array.at(s_t).is_string()) {
            r_s.color_palette.push_back(svg::Color(c_p_array.at(s_t).as_string()));
            continue;
        } else if(c_p_array.at(s_t).is_array()) {
            r_s.color_palette.push_back(set_rgb_rgba_color(c_p_array.at(s_t).as_array()));
            continue;
        }
    }
}


json_reader::JSONReader& json_reader::JSONReader::parse_render_settings(RenderSettings& r_s) {
    json::Dict ren_set = doc_being_proc.get_root().as_map().at("render_settings").as_map();
    r_s.width = ren_set["width"].as_double();
    r_s.height = ren_set["height"].as_double();

    if(ren_set["padding"].as_double() < 0) {
        r_s.padding = 0;
    } else {
        r_s.padding = ren_set["padding"].as_double();
    }

    r_s.line_width = ren_set["line_width"].as_double();
    r_s.stop_radius = ren_set["stop_radius"].as_double();

    r_s.bus_label_font_size = ren_set["bus_label_font_size"].as_double();
    for(size_t s_t = 0; s_t < ren_set["bus_label_offset"].as_array().size(); s_t++) {
        r_s.bus_label_offset.push_back(ren_set["bus_label_offset"].as_array()[s_t].as_double());
    }

    r_s.stop_label_font_size = ren_set["stop_label_font_size"].as_int();
    for(size_t s_t = 0; s_t < ren_set["stop_label_offset"].as_array().size(); s_t++) {
        r_s.stop_label_offset.push_back(ren_set["stop_label_offset"].as_array()[s_t].as_double());
    }

    set_color_pallete(ren_set["color_palette"].as_array(), r_s);

    for(size_t s_t = 0; s_t < ren_set["stop_label_offset"].as_array().size(); s_t++) {
        r_s.bus_label_offset.push_back(ren_set["stop_label_offset"].as_array()[s_t].as_double());
    }

    r_s.underlayer_width = ren_set["underlayer_width"].as_double();

    if(ren_set["underlayer_color"].is_string()) {
        r_s.underlayer_color = svg::Color(ren_set["underlayer_color"].as_string());
    } else if (ren_set["underlayer_color"].is_array() ) {
        r_s.underlayer_color  = set_rgb_rgba_color(ren_set["underlayer_color"].as_array());
    }
    return *this;
}


json::Dict json_reader::JSONReader::error_dict(int id) const {
    return {{"request_id", id},
        {"error_message", json::Node(err_msg)}};
}

std::string& json_reader::JSONReader::svg_to_json_format(std::string& text) {
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

json::Node json_reader::JSONReader::stop_req_processing(const std::map<std::string, json::Node>& map_node, const TransportCatalogue& t_c) const {
    if(t_c.pointer_stop_name(map_node.at("name").as_string()) == nullptr) {
        return error_dict(map_node.at("id").as_int());
    } else {
        const std::set<Bus*, bus_compare>& buses = t_c.stopname_to_buses(map_node.at("name").as_string());
        std::vector<json::Node> buses_name;
        for(const auto bus : buses) {
            buses_name.push_back(json::Node(bus->name_));
        }

        json::Dict result = {{"request_id", json::Node(map_node.at("id").as_int())},
            {"buses", json::Node(buses_name)}
        };
        return result;
    }
}

json::Node json_reader::JSONReader::bus_req_processing(const std::map<std::string, json::Node>& map_node, const BusInfo& b_i) const {
    json::Dict result = {{"request_id", json::Node(map_node.at("id").as_int())},
    {"curvature", b_i.curvature_},
    {"route_length", b_i.route_length_},
    {"stop_count", b_i.stop_count_},
    {"unique_stop_count", b_i.unique_stop_count_}};
    return result;
}

json::Node json_reader::JSONReader::svg_req_processing(const std::map<std::string, json::Node>& map_node, const std::string& svg_str) const {
    json::Dict result = {{"request_id", json::Node(map_node.at("id").as_int())}, {"map" , json::Node(svg_str)}};
    return result;
}


