#include "request_handler.h"



BusInfo RequestHandler::get_bus_info(Bus* bus) {
    long bus_route_lenght = t_c_.calculate_distance(bus);

    double bus_route_lenght_geo = 0;
    for(size_t t = 0; t < bus->route_.size() - 1 ; ++t) {
        bus_route_lenght_geo += geo::ComputeDistance({bus->route_[t]->latitude, bus->route_[t]->longitude}, {bus->route_[t+1]->latitude, bus->route_[t+1]->longitude});
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


void RequestHandler::print_map(std::ostream& out) {
     m_r_.print_route_line();
     m_r_.print_bus_name();
     m_r_.print_stop_circle();
     m_r_.print_stop_name();
     m_r_.get_svg_doc().Render(out);
}

void processing_requests(RequestHandler& r_h ,std::ostream& out) {
 //   std::vector<json::Node> result = j_p.json_stat_from_tc(r_h);
 //   json::PrintValue(result, out);
}

