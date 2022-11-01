#pragma once
#include<deque>
#include<unordered_map>
#include<string_view>
#include<string>
#include<vector>
#include<iostream>
#include<deque>
#include<algorithm>
#include<set>
#include<numeric>
// напишите решение с нуля
// код сохраните в свой git-репозиторий
class TransportCatalogue
{
public:
//Структура остановки, мувлю имя
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
        Bus(const std::string& n, std::vector<Stop*>& r, bool ring):route_(r),its_ring_(ring)
        {
            name_ = std::move(n);
        }

        size_t route_size();
        bool its_ring();

        std::string name_;
        std::vector<Stop*> route_;
        bool its_ring_;
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

    void add_stop(std::string& name, double lat, double lon);
    void add_distance_to_stop(std::string_view name1, std::string_view name2, long dist);
    std::deque<Stop>& stops_return();
    std::deque<Bus>& buses_return();
    Stop* pointer_stop_name(std::string_view name);
    Bus* pointer_bus_name(std::string_view name);
    const std::set<Bus*, bus_compare>& stopname_to_buses(const std::string_view name);
    void add_bus(std::string& name, std::vector<Stop*>& stops, bool ring);
    long distancse_2stops(std::string_view name1, std::string_view name2);
    bool exists_distance(std::string_view name1, std::string_view name2);
    long calculate_distance(Bus* bus);

private:
    std::deque<Stop> stops_;//контейнер существующих остановок
    std::unordered_map<std::string_view, Stop*> stopname_to_stop_;//контейнер для быстрого нахождения указателя на остановку по имени
    std::deque<Bus> buses_;//контейнер существующих остановок
    std::unordered_map<std::string_view, Bus*> busname_to_bus_;//контейнер для быстрого нахождения указателя на автобус по имени
    std::unordered_map<std::string_view, std::set<Bus*, bus_compare>> stopname_to_buses_;//контейнер для быстрого нахождения указателя на автобус по имени
    std::set<TransportCatalogue::Bus*, bus_compare> null_buses;
    std::unordered_map<std::pair<Stop*, Stop*>, long, StopPtrHasher> stops_distance_;
};






