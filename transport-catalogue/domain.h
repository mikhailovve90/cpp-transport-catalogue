#pragma once
#include <string>
#include <vector>
#include <set>
#include "graph.h"
/*
 * В этом файле вы можете разместить классы/структуры, которые являются частью предметной области (domain)
 * вашего приложения и не зависят от транспортного справочника. Например Автобусные маршруты и Остановки.
 *
 * Их можно было бы разместить и в transport_catalogue.h, однако вынесение их в отдельный
 * заголовочный файл может оказаться полезным, когда дело дойдёт до визуализации карты маршрутов:
 * визуализатор карты (map_renderer) можно будет сделать независящим от транспортного справочника.
 *
 * Если структура вашего приложения не позволяет так сделать, просто оставьте этот файл пустым.
 *
 */

struct Stop {
    Stop() {};
    Stop(const std::string& name, double lat, double lon): name_(name), latitude(lat), longitude(lon) {}
    std::string name_;
    double latitude;
    double longitude;
    graph::VertexId start_vertex;
    graph::VertexId route_vertex;
};

struct Bus {
    Bus() {};
    Bus(const std::string& name, const std::vector<Stop*>& stops, bool ring): name_(name), route_(stops), is_circular_route_(ring) {}

    size_t route_size() const;
    bool  is_circular_route() const;

    std::string name_;
    std::vector<Stop*> route_;
    bool  is_circular_route_;
};

struct BusInfo {
    BusInfo(double curvature, long route_length, size_t stop_count, size_t unique_stop_count) :
        curvature_(curvature),route_length_(route_length), stop_count_(stop_count), unique_stop_count_(unique_stop_count) {};
    double curvature_;
    int route_length_;
    int stop_count_;
    int unique_stop_count_;
};

struct StopPtrHasher {
    size_t operator() (std::pair<const Stop*, const Stop*> stop_pair) const {

        return (size_t)stop_pair.first + (size_t)stop_pair.second * 37;
    }
  private:
    std::hash<long> d_hasher_;
};

struct bus_compare {
    bool operator() (const Bus* lhs, const Bus* rhs) const {
        return lhs->name_ < rhs->name_;
    }
};

struct stop_compare {
    bool operator() (const Stop* lhs, const Stop* rhs) const {
        return lhs->name_ < rhs->name_;
    }
};
