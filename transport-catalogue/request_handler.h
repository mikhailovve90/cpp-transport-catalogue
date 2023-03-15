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
    RequestHandler(const TransportCatalogue& t_c, MapRenderer& m_r, json_reader::JSONReader& doc, bool init_router) : transport_catalogue_(t_c), map_renderer_(m_r), doc_(doc) {
         if(init_router){
            doc_.parse_routing_settings(transport_route_);
            //transport_route_.initialize_graph(transport_catalogue_.get_stops());
            //transport_route_.add_edges_buses_route_to_graph(transport_catalogue_);
            //transport_route_.initialize_router();
         }
    }
    BusInfo get_bus_info(Bus* bus);

    const TransportCatalogue& get_transport_catalogue_() {
        return transport_catalogue_;
    }
    const MapRenderer& get_map_renderer() {
        return map_renderer_;
    }

    TransportRouter& get_tansport_router() {
        return transport_route_;
    }

    void reinitialize_router();

    void render_map(std::ostream& out);
    std::vector<json::Node> process_requests();
    json::Node& building_node();

  private:
    const TransportCatalogue& transport_catalogue_;
    MapRenderer& map_renderer_;
    json_reader::JSONReader& doc_;
    std::vector<json::Node> answer_;
    json::Node build_node_;
    TransportRouter transport_route_;
};

//void process_requests(RequestHandler& r_h ,std::ostream& out, JSONReader& j_p);
