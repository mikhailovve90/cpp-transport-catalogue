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
//#include "json_reader.h"


class RequestHandler {
public:
    RequestHandler(const TransportCatalogue& t_c, MapRender& m_r) : t_c_(t_c), m_r_(m_r){}
    BusInfo get_bus_info(Bus* bus);

    const TransportCatalogue& get_ref_to_t_c() {return t_c_;}
    const MapRender& get_ref_to_m_r() {return m_r_;}
    void print_map(std::ostream& out);

private:
   const TransportCatalogue& t_c_;
   MapRender& m_r_;
};

//void processing_requests(RequestHandler& r_h ,std::ostream& out, JsonProcessing& j_p);
