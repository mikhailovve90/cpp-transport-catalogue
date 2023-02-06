#include "transport_router.h"
const std::optional<graph::Router<double>::RouteInfo> TransportRouter::get_route(const Stop* from, const Stop* to) const {
    std::optional<graph::Router<double>::RouteInfo> result = router_->BuildRoute(from->start_vertex, to->start_vertex);
    return result;
}

void TransportRouter::set_bus_speed(int bus_speed) {
    average_bus_speed_ = bus_speed;
}

void TransportRouter::set_wait_time(int wait_time) {
    wait_time_ = wait_time;
}
void TransportRouter::initialize_graph(const std::deque<Stop>& all_stops) {
    for(const Stop& stop : all_stops) {
        vertex_id_to_stop_[stop.start_vertex] = &stop;
        vertex_id_to_stop_[stop.route_vertex] = &stop;
    }
    size_t graph_size = all_stops.size() * 2;
    graph::DirectedWeightedGraph<double> init_graph(graph_size);
    bus_route_graph_= std::make_unique<graph::DirectedWeightedGraph<double>>(init_graph);
}

void TransportRouter::add_edges_buses_route_to_graph(const TransportCatalogue& t_c) {
    for(const Bus* bus : t_c.all_buses()) {
        add_bus_route_to_graph(bus, t_c);
    }
}

void TransportRouter::initialize_router() {
    graph::Router<double> init_router(*bus_route_graph_.get());
    router_ = std::make_unique<graph::Router<double>>(init_router);
}

void TransportRouter::add_circular_route_to_graph(const Bus* bus, const TransportCatalogue& t_c, const size_t route_size) {
    double integrated_time ;
    for(size_t i = 0; i < route_size - 2; ++i) {
        //Добавляю ребро от стартовой остановки до стартовой остановки плюс время ожидания
        graph::Edge<double> wait_edge{bus->route_[i]->start_vertex, bus->route_[i]->route_vertex, wait_time_};
        graph::EdgeId edge_id = bus_route_graph_->AddEdge(wait_edge);
        edge_id_to_bus_[edge_id] = std::pair(bus, std::deque{bus->route_[i]});
        integrated_time = 0;
        bool flag = false;
        size_t stops_count = 0;
        //для накопления остановок в в ребре
        std::deque<Stop*> stops_edge;
        stops_edge.push_back(bus->route_[i]);
        //минус три для поддержки старых запросов, когда конечная маршрута ещё не добавлялась в запросе последней, а добавлялась вручную
        for(size_t j = i + 1; stops_count < bus->route_size() - 3; ++j,++stops_count) {
            //Если достиг конца маршрута то с последняя остановка прибавляется, после этого ссаживаются пассажиры
            if(j == route_size - 2) {
                flag = true;
                integrated_time += (t_c.get_distance_between_stops_for_graph(bus->route_[route_size - 3]->name_, bus->route_[0]->name_) / (static_cast<double>(average_bus_speed_) * meters_in_kilometer)) * minutes_in_hour;
            } else {
                integrated_time += (t_c.get_distance_between_stops_for_graph(bus->route_[j - 1]->name_, bus->route_[j]->name_) / (static_cast<double>(average_bus_speed_) * meters_in_kilometer)) * minutes_in_hour;
            }
            graph::Edge<double> travel_edge{bus->route_[i]->route_vertex, bus->route_[j]->start_vertex, integrated_time};
            edge_id = bus_route_graph_->AddEdge(travel_edge);
            //Добавка в дек элемента который будет включен в ребро
            stops_edge.push_back(bus->route_[flag ? 0 : j]);
            edge_id_to_bus_[edge_id] = std::move(std::pair(bus, stops_edge));
            //Если мы достигли конца то завершаем
            if(flag) break;
        }
    }
}

void TransportRouter::add_linear_route_to_graph(const Bus* bus, const TransportCatalogue& t_c, const size_t route_size) {
        double integrated_time ;
        //Проход по прямому маршруту
        for(size_t i = 0; i <= route_size - 1; ++i) {
            graph::Edge<double> wait_edge{bus->route_[i]->start_vertex, bus->route_[i]->route_vertex, wait_time_};
            graph::EdgeId edge_id = bus_route_graph_->AddEdge(wait_edge);

            edge_id_to_bus_[edge_id] = std::pair(bus, std::deque{bus->route_[i]});
            integrated_time = 0;
            std::deque<Stop*> stops_edge;
            stops_edge.push_back(bus->route_[i]);
            for(size_t j = i + 1; j < bus->route_size(); ++j) {
                integrated_time += (t_c.get_distance_between_stops_for_graph(bus->route_[j - 1]->name_, bus->route_[j]->name_) / (static_cast<double>(average_bus_speed_) * meters_in_kilometer)) * minutes_in_hour;
                graph::Edge<double> travel_edge{bus->route_[i]->route_vertex, bus->route_[j]->start_vertex, integrated_time};
                edge_id = bus_route_graph_->AddEdge(travel_edge);
                stops_edge.push_back(bus->route_[j]);
                edge_id_to_bus_[edge_id] = std::move(std::pair(bus, stops_edge));
            }
        }
        //Проход по обратному маршруту
        for(int i = route_size - 1; i > 0; --i) {
            integrated_time = 0;
            std::deque<Stop*> stops_edge;
            stops_edge.push_back(bus->route_[i]);
            for(int j = i - 1; j >= 0; --j) {
                integrated_time += (t_c.get_distance_between_stops_for_graph(bus->route_[j+1]->name_, bus->route_[j]->name_) / (static_cast<double>(average_bus_speed_) * meters_in_kilometer))  * minutes_in_hour;
                graph::Edge<double> travel_edge{bus->route_[i]->route_vertex, bus->route_[j]->start_vertex, integrated_time};
                graph::EdgeId edge_id = bus_route_graph_->AddEdge(travel_edge);
                stops_edge.push_back(bus->route_[j]);
                edge_id_to_bus_[edge_id] = std::pair(bus, stops_edge);
            }
        }
}

void TransportRouter::add_bus_route_to_graph(const Bus* bus, const TransportCatalogue& t_c) {
    size_t route_size = bus->route_size();
    if(bus-> is_circular_route()) {
        add_circular_route_to_graph(bus, t_c, route_size);
    } else {
        add_linear_route_to_graph(bus, t_c, route_size);
    }
}
