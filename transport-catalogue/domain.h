#pragma once
#include <string>
#include <vector>
#include <set>
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

struct Stop
{
    Stop() {};
    Stop(const std::string& n, double lat, double lon):latitude(lat), longitude(lon)
    {
        name = std::move(n);
    }
    std::string name;
    double latitude;
    double longitude;
};

struct Bus
{
    Bus() {};
    Bus(const std::string& n, std::vector<Stop*>& r, bool ring):route_(r),is_ring_(ring)
    {
        name_ = std::move(n);
    }

    size_t route_size();
    bool is_ring();

    std::string name_;
    std::vector<Stop*> route_;
    bool is_ring_;
};

struct BusInfo{
    BusInfo(double c, long r, size_t s, size_t u) : curvature_(c),route_length_(r), stop_count_(s), unique_stop_count_(u){};
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

struct bus_compare
{
    bool operator() (const Bus* lhs, const Bus* rhs) const
    {
        return lhs->name_ < rhs->name_;
    }
};

struct stop_compare
{
    bool operator() (const Stop* lhs, const Stop* rhs) const
    {
        return lhs->name < rhs->name;
    }
};

std::set<Stop*, stop_compare> stops_in_alphabetical(const std::set<Bus*, bus_compare>& buses);
