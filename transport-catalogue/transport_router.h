#pragma once
#include "transport_catalogue.h"

struct EdgeInfo {
    const Bus* bus;
    const Stop* stop;
    uint32_t size;
};


class TransportRouter {
  public:
    void initialize_graph(const std::deque<Stop>& all_stops);
    void initialize_router();
    void add_bus_route_to_graph(const Bus* bus, const TransportCatalogue& t_c);
    void add_edges_buses_route_to_graph(const TransportCatalogue& t_c);
    void set_wait_time(int wait_time);
    void set_bus_speed(int bus_speed);
    void set_wait_time(double wait_time);
    void set_bus_speed(double bus_speed);
    double get_wait_time();
    double get_bus_speed();
    const std::optional<graph::Router<double>::RouteInfo> get_route(const Stop* from, const Stop* to) const;
    void add_circular_route_to_graph(const Bus* bus, const TransportCatalogue& t_c, const size_t route_size);
    void add_linear_route_to_graph(const Bus* bus, const TransportCatalogue& t_c, const size_t route_size);

    const graph::Edge<double>& get_edge(graph::EdgeId edge_id) const {
        return bus_route_graph_->GetEdge(edge_id);
    }

    const size_t get_edge_count() const {
        return bus_route_graph_->GetEdgeCount();
    }

    const EdgeInfo& get_edge_content_from_edge_id(const graph::EdgeId id) const {
        return edge_id_to_bus_.at(id);
    }
  private:
    graph::VertexId vertex_counter_ = 0;
    std::unique_ptr<graph::DirectedWeightedGraph<double>> bus_route_graph_;
    std::unique_ptr<graph::Router<double>> router_;
    std::unordered_map<graph::VertexId, const Stop*> vertex_id_to_stop_;

    std::unordered_map<graph::EdgeId, EdgeInfo> edge_id_to_bus_;
    double wait_time_;
    double average_bus_speed_;
    const double meters_in_kilometer = 1000.0;
    const double minutes_in_hour = 60.0;
};
