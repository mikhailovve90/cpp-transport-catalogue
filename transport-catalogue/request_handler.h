#pragma once
#include <string>
#include <iostream>
#include <deque>
#include <unordered_map>
#include "transport_catalogue.h"
#include "geo.h"
#include <vector>
#include <numeric>
#include <algorithm>
#include <iomanip>
#include "map_renderer.h"
#include "json.h"
#include "json_reader.h"
#include "json_builder.h"
#include "transport_router.h"

class RequestHandler {
  public:
    RequestHandler(const TransportCatalogue& t_c, MapRenderer& m_r, json_reader::JSONReader& doc) : t_c_(t_c), m_r_(m_r), doc_(doc) {
        doc_.parse_routing_settings(t_r_);
        t_r_.initialize_graph(t_c_.get_stops());
        t_r_.add_edges_buses_route_to_graf(t_c_);
        t_r_.initialize_router();
    }
    BusInfo get_bus_info(Bus* bus);

    const TransportCatalogue& get_transport_catalogue() {
        return t_c_;
    }
    const MapRenderer& get_map_renderer() {
        return m_r_;
    }
    void render_map(std::ostream& out);
    std::vector<json::Node> process_requests();
    json::Node& building_node();

  private:
    const TransportCatalogue& t_c_;
    MapRenderer& m_r_;
    json_reader::JSONReader& doc_;
    std::vector<json::Node> answer_;
    json::Node build_node_;
    TransportRouter t_r_;
};

//void process_requests(RequestHandler& r_h ,std::ostream& out, JSONReader& j_p);
