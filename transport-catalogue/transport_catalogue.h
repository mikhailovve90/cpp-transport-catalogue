#pragma once
#include <deque>
#include <unordered_map>
#include <string_view>
#include <string>
#include <vector>
#include <iostream>
#include <deque>
#include <algorithm>
#include <set>
#include <numeric>
#include "domain.h"

// напишите решение с нуля
// код сохраните в свой git-репозиторий
class TransportCatalogue {
  public:
//Структура остановки, мувлю имя


    void add_stop(const std::string& name, double lat, double lon);
    void add_distance_to_stop(std::string_view name1, std::string_view name2, long dist);
    std::deque<Stop>& stops_return();
    std::deque<Bus>& buses_return();
    Stop* pointer_stop_name(std::string_view name);
    Bus* pointer_bus_name(std::string_view name);
    const std::set<Bus*, bus_compare>& stopname_to_buses(const std::string_view name);
    void add_bus(const std::string& name, std::vector<Stop*>& stops, bool ring);
    long distancse_2stops(std::string_view name1, std::string_view name2);
    bool exists_distance(std::string_view name1, std::string_view name2);
    long calculate_distance(Bus* bus);
    const std::set<Bus*, bus_compare>& all_buses();

  private:
    std::deque<Stop> stops_;//контейнер существующих остановок
    std::unordered_map<std::string_view, Stop*> stopname_to_stop_;//контейнер для быстрого нахождения указателя на остановку по имени
    std::deque<Bus> buses_;//контейнер существующих остановок
    std::unordered_map<std::string_view, Bus*> busname_to_bus_;//контейнер для быстрого нахождения указателя на автобус по имени
    std::unordered_map<std::string_view, std::set<Bus*, bus_compare>> stopname_to_buses_;//контейнер для быстрого нахождения указателя на автобус по имени
    std::set<Bus*, bus_compare> null_buses;
    std::unordered_map<std::pair<Stop*, Stop*>, long, StopPtrHasher> stops_distance_;
    std::set<Bus*, bus_compare> all_buses_;
};
