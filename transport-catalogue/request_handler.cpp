#include "request_handler.h"



BusInfo RequestHandler::get_bus_info(Bus* bus) {
    long bus_route_lenght = t_c_.calculate_distance(bus);

    double bus_route_lenght_geo = 0;
    for(size_t t = 0; t < bus->route_.size() - 1 ; ++t) {
        bus_route_lenght_geo += geo::compute_distance({bus->route_[t]->latitude, bus->route_[t]->longitude}, {bus->route_[t+1]->latitude, bus->route_[t+1]->longitude});
    }
    if(!bus->its_ring()) {
        bus_route_lenght_geo *= 2;
    }

    size_t route_size = bus->its_ring()?bus->route_size()-1:bus->route_size()*2 - 1;

    auto copy_route = bus->route_;
    size_t uniq_stops_count;
    std::sort(copy_route.begin(), copy_route.end(),
    [](const Stop* a,const Stop* b) {
        return std::lexicographical_compare(a->name.begin(), a->name.end(), b->name.begin(), b->name.end());
    });
    uniq_stops_count = distance(copy_route.begin(),std::unique(copy_route.begin(),copy_route.end()));

    double curva = bus_route_lenght/bus_route_lenght_geo;

    return BusInfo(curva, bus_route_lenght, route_size, uniq_stops_count);
}


void RequestHandler::render_map(std::ostream& out) {
     m_r_.render_route_line();
     m_r_.render_bus_name();
     m_r_.render_stop_circle();
     m_r_.render_stop_name();
     m_r_.get_svg_doc().Render(out);
}

std::vector<json::Node> RequestHandler::processing_requests() {
    std::vector<json::Node> requests = doc_.get_doc().get_root().as_map().at("stat_requests").as_array();

    std::vector<json::Node> result;
    for(const json::Node& n_d : requests) {
        auto& map_node = n_d.as_map();
        if(map_node.at("type").as_string() == "Stop") {
            if(t_c_.pointer_stop_name(map_node.at("name").as_string()) == nullptr) {
                result.push_back(doc_.error_dict(map_node.at("id").as_int()));
            } else {
                const std::set<Bus*, bus_compare>& buses = t_c_.stopname_to_buses(map_node.at("name").as_string());
                std::vector<json::Node> buses_name;
                for(const auto bus : buses) {
                    buses_name.push_back(json::Node(bus->name_));
                }

                json::Dict good_req = {{"request_id", json::Node(map_node.at("id").as_int())},
                    {"buses", json::Node(buses_name)}
                };
                result.push_back(good_req);
            }
        } else if(map_node.at("type").as_string() == "Bus") {
            if(t_c_.pointer_bus_name(map_node.at("name").as_string()) == nullptr) {
                result.push_back(doc_.error_dict(map_node.at("id").as_int()));
            } else {
                const auto a = map_node.at("name").as_string();
                BusInfo b_i = this->get_bus_info(t_c_.pointer_bus_name(map_node.at("name").as_string()));

                json::Dict good_req = {{"request_id", json::Node(map_node.at("id").as_int())},
                    {"curvature", b_i.curvature_},
                    {"route_length", b_i.route_length_},
                    {"stop_count", b_i.stop_count_},
                    {"unique_stop_count", b_i.unique_stop_count_}
                };

                result.push_back(good_req);
            }
        } else if(map_node.at("type").as_string() == "Map") {
            svg::Document svg_doc;
            std::ostringstream out;
            this->render_map(out);
            std::string svg_str = out.str();
            doc_.svg_to_json_format(svg_str);

            json::Dict svg_dict = {{"request_id", json::Node(map_node.at("id").as_int())}, {"map" , json::Node(svg_str)}};

            result.push_back(svg_dict);
        }
    }
    return result;
}


