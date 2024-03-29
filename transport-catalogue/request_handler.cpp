#include "request_handler.h"



BusInfo RequestHandler::get_bus_info(Bus* bus) {
    long bus_route_lenght = transport_catalogue_.calculate_distance(bus);

    double bus_route_lenght_geo = 0;
    for(size_t t = 0; t < bus->route_.size() - 1 ; ++t) {
        bus_route_lenght_geo += geo::compute_distance({bus->route_[t]->latitude, bus->route_[t]->longitude}, {bus->route_[t+1]->latitude, bus->route_[t+1]->longitude});
    }
    if(!bus-> is_circular_route()) {
        bus_route_lenght_geo *= 2;
    }

    size_t route_size = bus-> is_circular_route()?bus->route_size()-1:bus->route_size()*2 - 1;

    auto copy_route = bus->route_;
    size_t uniq_stops_count;
    std::sort(copy_route.begin(), copy_route.end(),
    [](const Stop* a,const Stop* b) {
        return std::lexicographical_compare(a->name_.begin(), a->name_.end(), b->name_.begin(), b->name_.end());
    });
    uniq_stops_count = distance(copy_route.begin(),std::unique(copy_route.begin(),copy_route.end()));

    double curva = bus_route_lenght/bus_route_lenght_geo;

    return BusInfo(curva, bus_route_lenght, route_size, uniq_stops_count);
}


void RequestHandler::render_map(std::ostream& out) {
    map_renderer_.render_route_line();
    map_renderer_.render_bus_name();
    map_renderer_.render_stop_circle();
    map_renderer_.render_stop_name();
    map_renderer_.get_svg_doc().Render(out);
}

std::vector<json::Node> RequestHandler::process_requests() {
    answer_.clear();
    std::vector<json::Node> requests = doc_.get_doc().get_root().as_map().at("stat_requests").as_array();

    std::vector<json::Node> result;
    for(const json::Node& n_d : requests) {
        const std::map<std::string, json::Node>& map_node = n_d.as_map();
        if(map_node.at("type").as_string() == "Stop") {
            answer_.push_back(doc_.stop_req_processing(map_node, transport_catalogue_));
        } else if(map_node.at("type").as_string() == "Bus") {
            if(transport_catalogue_.pointer_bus_name(map_node.at("name").as_string()) == nullptr) {
                answer_.push_back(doc_.error_dict(map_node.at("id").as_int()));
            } else {
                const auto a = map_node.at("name").as_string();
                BusInfo b_i = this->get_bus_info(transport_catalogue_.pointer_bus_name(map_node.at("name").as_string()));
                answer_.push_back(doc_.bus_req_processing(map_node, b_i));
            }
        } else if(map_node.at("type").as_string() == "Map") {
            svg::Document svg_doc;
            std::ostringstream out;
            this->render_map(out);
            std::string svg_str = out.str();
            doc_.svg_to_json_format(svg_str);
            answer_.push_back(doc_.svg_req_processing(map_node, svg_str));
        } else if((map_node.at("type").as_string() == "Route")) {
            Stop* from = transport_catalogue_.pointer_stop_name(map_node.at("from").as_string());
            Stop* to = transport_catalogue_.pointer_stop_name(map_node.at("to").as_string());;
            std::optional<graph::Router<double>::RouteInfo> result = transport_route_.get_route(from, to);

            if(!result.has_value()) {
                answer_.push_back(doc_.error_dict(map_node.at("id").as_int()));
            } else {
                answer_.push_back(doc_.route_req_processing(map_node, transport_route_, result));
            }
        }
    }
    return answer_;
}

json::Node& RequestHandler::building_node() {
    json::Builder builder;
    builder.StartArray();
    for(auto node : answer_) {
        if(node.is_map()) {
            builder.StartDict();
            for(auto [key, value] : node.as_map()) {
                builder.Key(key).Value(value);
            }
            builder.EndDict();
        }
    }

    builder.EndArray();
    build_node_ = builder.Build();
    return build_node_;
}

void RequestHandler::reinitialize_router() {
    transport_route_.initialize_graph(transport_catalogue_.get_stops());
    transport_route_.add_edges_buses_route_to_graph(transport_catalogue_);
    transport_route_.initialize_router();
}
