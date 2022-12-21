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


class RequestHandler {
public:
    RequestHandler(const TransportCatalogue& t_c, MapRenderer& m_r, json_reader::JSONReader& doc) : t_c_(t_c), m_r_(m_r), doc_(doc){}
    BusInfo get_bus_info(Bus* bus);

    const TransportCatalogue& get_transport_catalogue() {return t_c_;}
    const MapRenderer& get_map_renderer() {return m_r_;}
    void render_map(std::ostream& out);
    std::vector<json::Node> process_requests();

private:
   const TransportCatalogue& t_c_;
   MapRenderer& m_r_;
   json_reader::JSONReader& doc_;
   std::vector<json::Node> answer_;
};

//void process_requests(RequestHandler& r_h ,std::ostream& out, JSONReader& j_p);
