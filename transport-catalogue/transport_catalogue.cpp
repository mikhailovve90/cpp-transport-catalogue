#include "transport_catalogue.h"

void TransportCatalogue::add_stop(std::string& name, double lat, double lon)
{
    Stop add_stop(name, lat, lon);
    stops_.push_back(add_stop);
    stopname_to_stop_[stops_.back().name] = &stops_.back();
}

void TransportCatalogue::add_distance_to_stop(std::string_view name1, std::string_view name2, long dist)
{
      //std::cout << "NAM1" << name1 << "NAM2" << name2 << "DIST" << dist << "FF" << std::endl;
      stops_distance_[std::pair(pointer_stop_name(name1), pointer_stop_name(name2))] = dist;
}

std::deque<TransportCatalogue::Stop>& TransportCatalogue::stops_return()
{
  return stops_;
}

std::deque<TransportCatalogue::Bus>& TransportCatalogue::buses_return()
{
  return buses_;
}

TransportCatalogue::Stop* TransportCatalogue::pointer_stop_name(std::string_view name)
{
    if(stopname_to_stop_.count(name))
    {
        return stopname_to_stop_[name];
    }
    return nullptr;
}

TransportCatalogue::Bus* TransportCatalogue::pointer_bus_name(std::string_view name)
{
    if(busname_to_bus_.count(name))
    {
        return busname_to_bus_[name];
    }
    return nullptr;
}

void TransportCatalogue::add_bus(std::string& name, std::vector<Stop*>& stops, bool ring)
{
    Bus add_bus(name, stops, ring);
    buses_.push_back(add_bus);
    busname_to_bus_[buses_.back().name_] = &buses_.back();
    //size_t sub_if_ring = ring ? 1 : 0;
    for(size_t t = 0; t < stops.size(); ++t)
    {
        {
            stopname_to_buses_[stopname_to_stop_.at(stops[t]->name)->name].insert(&buses_.back());//.push_back(&buses_.back());
        }
    }
}

const std::set<TransportCatalogue::Bus*, TransportCatalogue::bus_compare>& TransportCatalogue::stopname_to_buses(const std::string_view name)
{
    if(stopname_to_buses_.count(name))
    {
        return stopname_to_buses_[name];
    }
    return null_buses;
}

size_t TransportCatalogue::Bus::route_size()
{
    return route_.size();
}

bool TransportCatalogue::Bus::its_ring()
{
    return its_ring_;
}

long TransportCatalogue::distancse_2stops(std::string_view name1, std::string_view name2)
{
    if(exists_distance(name1, name2))
    {
        return stops_distance_[std::pair(pointer_stop_name(name1), pointer_stop_name(name2))];
    }
    return 0;
}

bool TransportCatalogue::exists_distance(std::string_view name1, std::string_view name2)
{
        Stop* stop1 = pointer_stop_name(name1);
        Stop* stop2 = pointer_stop_name(name2);
        return stops_distance_.count(std::pair(stop1, stop2));
}

long TransportCatalogue::calculate_distance(Bus* bus)
{
    long result = 0;
    if(bus->its_ring())
    {
        for(size_t i = 0; i < bus->route_.size() - 1; ++i)
        {
            /*std::cout << "lol" << bus->route_[i]->name << "lol2" << bus->route_[i + 1]->name << "lil"
             << distancse_2stops(bus->route_[i]->name, bus->route_[i + 1]->name) << "xuita" << distancse_2stops(bus->route_[i + 1]->name, bus->route_[i]->name) << std::endl;*/
            result += exists_distance(bus->route_[i]->name, bus->route_[i + 1]->name) ? distancse_2stops(bus->route_[i]->name, bus->route_[i + 1]->name) : distancse_2stops(bus->route_[i + 1]->name, bus->route_[i]->name);
        }
        result += distancse_2stops(bus->route_[0]->name, bus->route_[0]->name);
    } else
    {
        for(size_t i = 0; i < bus->route_.size() - 1; ++i)
        {
            result += exists_distance(bus->route_[i]->name, bus->route_[i + 1]->name) ? distancse_2stops(bus->route_[i]->name, bus->route_[i + 1]->name) : distancse_2stops(bus->route_[i + 1]->name, bus->route_[i]->name);
            result += exists_distance(bus->route_[i + 1]->name, bus->route_[i]->name) ? distancse_2stops(bus->route_[i + 1]->name, bus->route_[i]->name) : distancse_2stops(bus->route_[i]->name, bus->route_[i + 1]->name);
        }
    }
    return result;
}
