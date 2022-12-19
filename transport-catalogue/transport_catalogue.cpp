#include "transport_catalogue.h"

#include "transport_catalogue.h"

void TransportCatalogue::add_stop(const std::string& name, double lat, double lon) {
    Stop add_stop(name, lat, lon);
    stops_.push_back(add_stop);
    stopname_to_stop_[stops_.back().name] = &stops_.back();
}

void TransportCatalogue::add_distance_to_stop(std::string_view name1, std::string_view name2, long dist) {

    stops_distance_[std::pair(pointer_stop_name(name1), pointer_stop_name(name2))] = dist;
}

std::deque<Stop>& TransportCatalogue::get_stops() {
    return stops_;
}

std::deque<Bus>& TransportCatalogue::get_buses() {
    return buses_;
}

Stop* TransportCatalogue::pointer_stop_name(std::string_view name) const{
    if(stopname_to_stop_.count(name)) {
        return stopname_to_stop_.at(name);
    }
    return nullptr;
}

Bus* TransportCatalogue::pointer_bus_name(std::string_view name) const {
    if(busname_to_bus_.count(name)) {
        return busname_to_bus_.at(name);
    }
    return nullptr;
}

const std::set<Bus*, bus_compare>& TransportCatalogue::all_buses() const {
    return all_buses_;
}

void TransportCatalogue::add_bus(const std::string& name, std::vector<Stop*>& stops, bool ring) {
    Bus add_bus(name, stops, ring);
    buses_.push_back(add_bus);
    busname_to_bus_[buses_.back().name_] = &buses_.back();
    //size_t sub_if_ring = ring ? 1 : 0;
    all_buses_.insert(&buses_.back());
    for(size_t t = 0; t < stops.size(); ++t) {
        {
            stopname_to_buses_[stopname_to_stop_.at(stops[t]->name)->name].insert(&buses_.back());//.push_back(&buses_.back());
        }
    }
}

const std::set<Bus*, bus_compare>& TransportCatalogue::stopname_to_buses(const std::string_view name) const {
    if(stopname_to_buses_.count(name)) {
        return stopname_to_buses_.at(name);
    }
    return null_buses;
}

long TransportCatalogue::get_distance_between_stops(std::string_view name1, std::string_view name2) const{
    if(exists_distance(name1, name2)) {
        return stops_distance_.at(std::pair(pointer_stop_name(name1), pointer_stop_name(name2)));
    }
    return 0;
}

bool TransportCatalogue::exists_distance(std::string_view name1, std::string_view name2) const {
    Stop* stop1 = pointer_stop_name(name1);
    Stop* stop2 = pointer_stop_name(name2);
    return stops_distance_.count(std::pair(stop1, stop2));
}

long TransportCatalogue::calculate_distance(Bus* bus) const{
    long result = 0;
    if(bus->its_ring()) {
        for(size_t i = 0; i < bus->route_.size() - 1; ++i) {
            result += exists_distance(bus->route_.at(i)->name, bus->route_.at(i + 1)->name) ?
            get_distance_between_stops(bus->route_.at(i)->name, bus->route_.at(i + 1)->name) : get_distance_between_stops(bus->route_.at(i)->name, bus->route_.at(i)->name);
        }
        result += exists_distance(bus->route_.at(0)->name, bus->route_.at(bus->route_.size() - 1)->name) ?
                  get_distance_between_stops(bus->route_.at(0)->name, bus->route_.at(bus->route_.size() - 1)->name)
                  : get_distance_between_stops(bus->route_.at(bus->route_.size() - 1)->name, bus->route_.at(0)->name);
        result += get_distance_between_stops(bus->route_.at(0)->name, bus->route_.at(0)->name);
    } else {
        for(size_t i = 0; i < bus->route_.size() - 1; ++i) {
            result += exists_distance(bus->route_.at(i)->name, bus->route_.at(i + 1)->name) ?
            get_distance_between_stops(bus->route_.at(i)->name, bus->route_.at(i + 1)->name) : get_distance_between_stops(bus->route_.at(i + 1)->name, bus->route_.at(i)->name);
            result += exists_distance(bus->route_.at(i + 1)->name, bus->route_.at(i)->name) ? get_distance_between_stops(bus->route_.at(i + 1)->name, bus->route_.at(i)->name) :
            get_distance_between_stops(bus->route_.at(i)->name, bus->route_.at(i+ 1)->name);
        }
    }
    return result;
}
