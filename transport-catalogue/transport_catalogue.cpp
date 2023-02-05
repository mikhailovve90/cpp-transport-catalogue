#include "transport_catalogue.h"


void TransportCatalogue::add_stop(const std::string& name, double lat, double lon) {
    Stop add_stop(name, lat, lon);
    add_stop.start_vertex = vertex_counter_;
    ++vertex_counter_;
    add_stop.route_vertex = vertex_counter_;
    ++vertex_counter_;
    stops_.push_back(add_stop);
    stopname_to_stop_[stops_.back().name_] = &stops_.back();
}

void TransportCatalogue::add_distance_to_stop(std::string_view name1, std::string_view name2, long dist) {

    stops_distance_[std::pair(pointer_stop_name(name1), pointer_stop_name(name2))] = dist;
}

const std::deque<Stop>& TransportCatalogue::get_stops() const {
    return stops_;
}

const std::deque<Bus>& TransportCatalogue::get_buses() const {
    return buses_;
}

Stop* TransportCatalogue::pointer_stop_name(std::string_view name) const {
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
            stopname_to_buses_[stopname_to_stop_.at(stops[t]->name_)->name_].insert(&buses_.back());//.push_back(&buses_.back());
        }
    }
}

const std::set<Bus*, bus_compare>& TransportCatalogue::stopname_to_buses(const std::string_view name) const {
    if(stopname_to_buses_.count(name)) {
        return stopname_to_buses_.at(name);
    }
    return null_buses;
}

long TransportCatalogue::get_distance_between_stops(std::string_view name1, std::string_view name2) const {
    if(exists_distance(name1, name2)) {
        return stops_distance_.at(std::pair(pointer_stop_name(name1), pointer_stop_name(name2)));
    }
    return 0;
}

long TransportCatalogue::get_distance_between_stops_for_graph(std::string_view name1, std::string_view name2) const {
    if(exists_distance(name1, name2)) {
        return stops_distance_.at(std::pair(pointer_stop_name(name1), pointer_stop_name(name2)));
    } else if(exists_distance(name2, name1)) {
        return stops_distance_.at(std::pair(pointer_stop_name(name2), pointer_stop_name(name1)));
    }
    return 0;
}

bool TransportCatalogue::exists_distance(std::string_view name1, std::string_view name2) const {
    Stop* stop1 = pointer_stop_name(name1);
    Stop* stop2 = pointer_stop_name(name2);
    return stops_distance_.count(std::pair(stop1, stop2));
}

long TransportCatalogue::calculate_distance(Bus* bus) const {
    long result = 0;
    if(bus->is_ring()) {
        for(size_t i = 0; i < bus->route_.size() - 1; ++i) {
            result += exists_distance(bus->route_.at(i)->name_, bus->route_.at(i + 1)->name_) ?
                      get_distance_between_stops(bus->route_.at(i)->name_, bus->route_.at(i + 1)->name_) : get_distance_between_stops(bus->route_.at(i+1)->name_, bus->route_.at(i)->name_);
        }
        result += exists_distance(bus->route_.at(0)->name_, bus->route_.at(bus->route_.size() - 1)->name_) ?
                  get_distance_between_stops(bus->route_.at(0)->name_, bus->route_.at(bus->route_.size() - 1)->name_)
                  : get_distance_between_stops(bus->route_.at(bus->route_.size() - 1)->name_, bus->route_.at(0)->name_);
        result += get_distance_between_stops(bus->route_.at(0)->name_, bus->route_.at(0)->name_);
    } else {
        for(size_t i = 0; i < bus->route_.size() - 1; ++i) {
            result += exists_distance(bus->route_.at(i)->name_, bus->route_.at(i + 1)->name_) ?
                      get_distance_between_stops(bus->route_.at(i)->name_, bus->route_.at(i + 1)->name_) : get_distance_between_stops(bus->route_.at(i + 1)->name_, bus->route_.at(i)->name_);
            result += exists_distance(bus->route_.at(i + 1)->name_, bus->route_.at(i)->name_) ? get_distance_between_stops(bus->route_.at(i + 1)->name_, bus->route_.at(i)->name_) :
                      get_distance_between_stops(bus->route_.at(i)->name_, bus->route_.at(i+ 1)->name_);
        }
    }
    return result;
}

std::set<Stop*, stop_compare> TransportCatalogue::stops_in_alphabetical(const std::set<Bus*, bus_compare>& buses) const {
    std::set<Stop*, stop_compare> stops_in_alphabetical_;
    for(const auto bus : buses) {
        for(const auto stop : bus->route_) {
            stops_in_alphabetical_.insert(stop);
        }
    }
    return stops_in_alphabetical_;
}

