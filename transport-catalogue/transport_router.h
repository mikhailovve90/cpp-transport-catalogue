#pragma once
#include "transport_catalogue.h"

class TransportRouter {
  public:
    void initialize_graph(const std::deque<Stop>& all_stops);
    void initialize_router();
    void add_bus_route_to_graph(const Bus* bus, const TransportCatalogue& t_c);
    void add_edges_buses_route_to_graf(const TransportCatalogue& t_c);
    void set_wait_time(int wait_time);
    void set_bus_speed(int bus_speed);
    const std::optional<graph::Router<double>::RouteInfo> get_route(const Stop* from, const Stop* to) const;

    const graph::Edge<double>& get_edge(graph::EdgeId edge_id) const {
        return bus_route_graph_->GetEdge(edge_id);
    }

    const std::pair<const Bus*, std::deque<Stop*>>& get_edge_content_from_edge_id(const graph::EdgeId id) const {
        return edge_id_to_bus_.at(id);
    }
  private:
    graph::VertexId vertex_counter_ = 0;
    std::unique_ptr<graph::DirectedWeightedGraph<double>> bus_route_graph_;
    std::unique_ptr<graph::Router<double>> router_;
    std::unordered_map<graph::VertexId, const Stop*> vertex_id_to_stop_;
    std::unordered_map<graph::EdgeId, std::pair<const Bus*, std::deque<Stop*>>> edge_id_to_bus_;
    double wait_time_;
    double average_bus_speed_;
};
