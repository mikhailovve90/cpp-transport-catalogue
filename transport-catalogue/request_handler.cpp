#include "request_handler.h"

/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 *
 * Если вы затрудняетесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
 */
void t_c_o_r::read_processing_queue(TransportCatalogue& t_c, std::istream& input_stream, std::ostream& output_stream) {
    int count_query = t_c_i_r::read_line_with_number(input_stream);
    for(int i = 0; i < count_query; ++i) {
        std::string str = t_c_i_r::read_line(input_stream);
        t_c_i_r::Query query = t_c_i_r::string_to_query(str);
        std::string name = t_c_o_r::bstrip(query.text.substr(query.text.find_first_not_of(' ')));
        if(query.type == t_c_i_r::QueryType::BUS) {
            if(t_c.pointer_bus_name(name) != nullptr) {
                bus_information(t_c.pointer_bus_name(name), t_c, output_stream);
            } else {
                std::cout << "Bus " << name << ": not found" << std::endl;
            }
        } else if(query.type == t_c_i_r::QueryType::STOP) {
            if(t_c.pointer_stop_name(name) == nullptr) {
                std::cout << "Stop " << name << ": not found" << std::endl;;

            } else if(t_c.stopname_to_buses(name).empty()) {
                std::cout << "Stop " << name << ": no buses" << std::endl;
            } else {
                std::cout << "Stop " << name << ": buses";
                stop_information(t_c.stopname_to_buses(name), output_stream);
            }
        }
    }
}

BusInfo t_c_o_r::get_bus_info(Bus* bus, TransportCatalogue& t_c) {
    long bus_route_lenght = t_c.calculate_distance(bus);

    double bus_route_lenght_geo = 0;
    for(size_t t = 0; t < bus->route_.size() - 1 ; ++t) {
        bus_route_lenght_geo += geo::ComputeDistance({bus->route_[t]->latitude, bus->route_[t]->longitude}, {bus->route_[t+1]->latitude, bus->route_[t+1]->longitude});
    }
    if(!bus->its_ring()) {
        bus_route_lenght_geo *= 2;
    }

    size_t route_size = bus->its_ring()?bus->route_size()-1:bus->route_size()*2 - 1;

    auto copy_route = bus->route_;
    size_t uniq_stops_count;
    std::sort(copy_route.begin(), copy_route.end(),
    [](const Stop* a,const Stop* b) {
        return std::lexicographical_compare(a->name.begin(), a->name.end(), b->name.begin(), b->name.end());
    });
    uniq_stops_count = distance(copy_route.begin(),std::unique(copy_route.begin(),copy_route.end()));

    double curva = bus_route_lenght/bus_route_lenght_geo;

    return BusInfo(curva, bus_route_lenght, route_size, uniq_stops_count);
}

void t_c_o_r::bus_information(Bus* bus, TransportCatalogue& t_c, std::ostream& output_stream) {

    long bus_route_lenght = t_c.calculate_distance(bus);

    double bus_route_lenght_geo = 0;
    for(size_t t = 0; t < bus->route_.size() - 1 ; ++t) {
        bus_route_lenght_geo += geo::ComputeDistance({bus->route_[t]->latitude, bus->route_[t]->longitude}, {bus->route_[t+1]->latitude, bus->route_[t+1]->longitude});
    }
    if(!bus->its_ring()) {
        bus_route_lenght_geo *= 2;
    }

    size_t route_size = bus->its_ring()?bus->route_size():bus->route_size()*2 - 1;

    auto copy_route = bus->route_;
    size_t uniq_stops_count;
    std::sort(copy_route.begin(), copy_route.end(),
    [](const Stop* a,const Stop* b) {
        return std::lexicographical_compare(a->name.begin(), a->name.end(), b->name.begin(), b->name.end());
    });
    uniq_stops_count = distance(copy_route.begin(),std::unique(copy_route.begin(),copy_route.end()));

    output_stream << std::setprecision(6) << "Bus " << bus->name_ << ": " << route_size << " stops on route, "
                  << uniq_stops_count << " unique stops, "
                  << static_cast<double>(bus_route_lenght) << " route length"
                  <<  ", " << bus_route_lenght/bus_route_lenght_geo << " curvature" << std::endl;
}

void t_c_o_r::stop_information(const std::set<Bus*, bus_compare>& buses, std::ostream& output_stream) {
    for(const auto bus : buses) {
        output_stream << " " << bus->name_;
    }
    output_stream << std::endl;
}

std::string t_c_o_r::bstrip(std::string line) {
    while (!line.empty() && isspace(line[line.size()-1])) {
        line.pop_back();
    }
    return line;
}

std::string t_c_i_r::read_line(std::istream& input_stream) {
    std::string s;
    getline(input_stream, s);
    return s;
}

int t_c_i_r::read_line_with_number(std::istream& input_stream) {
    int result = std::stoi(t_c_i_r::read_line(input_stream));

    return result;
}

t_c_i_r::Query t_c_i_r::string_to_query(std::string& str_query) {
    t_c_i_r::Query result;
    int pos = str_query.find(' ');
    std::string sub_str = str_query.substr(0, pos);
    if(sub_str == "Bus") {
        result.type = t_c_i_r::QueryType::BUS;
    } else if(sub_str == "Stop") {
        result.type = t_c_i_r::QueryType::STOP;
    }
    result.text = std::move(str_query.substr(pos + 1));
    return result;
}

std::unordered_map<t_c_i_r::QueryType, std::deque<t_c_i_r::Query>> t_c_i_r::read_query_queue(std::istream& input_stream) {
    std::unordered_map<t_c_i_r::QueryType, std::deque<t_c_i_r::Query>> result;
    int count_query = t_c_i_r::read_line_with_number(input_stream);
    for(int i = 0; i < count_query; ++i) {
        std::string str = t_c_i_r::read_line(input_stream);
        auto query = t_c_i_r::string_to_query(str);
        result[query.type].push_back(query);
    }
    return result;
}

//Обработка запросов на добавление остановок
void t_c_i_r::processing_query_queue(std::unordered_map<QueryType, std::deque<Query>>& q_q, TransportCatalogue& t_c) {
    std::unordered_map<std::string_view, std::pair<Query*, long >> dist_query_queue;
    size_t counter = 0;
    for(const t_c_i_r::Query& str_query : q_q[t_c_i_r::QueryType::STOP]) { //иду по запросам на добавление остановок
        int pos = str_query.text.find(':');
        std::string name = std::move(str_query.text.substr(0, pos));

        int pos_comm = str_query.text.find(',');
        double latitude = std::stod(str_query.text.substr(pos+1, pos_comm));//из строки выцепляю широту
        pos = str_query.text.find(',', pos_comm + 1);
        double longitude = std::stod(str_query.text.substr(pos_comm+1, pos - pos_comm));//из строки выцепляю долготу
        t_c.add_stop(name, latitude, longitude);
        dist_query_queue[t_c.pointer_stop_name(name)->name] = std::pair(&q_q[t_c_i_r::QueryType::STOP][counter], pos + 1);
        ++counter;
    }


    for(const t_c_i_r::Query& str_query : q_q[t_c_i_r::QueryType::BUS]) { //иду по запросам на добавление остановок
        std::vector<Stop*> route;
        auto pos = str_query.text.find(':');
        std::string name = std::move(str_query.text.substr(0, pos));

        auto pos_comm = str_query.text.find('>');
        if(pos_comm != std::string::npos) {
            std::string str_without_name = str_query.text.substr(pos+2);
            std::vector<Stop*> stops = read_route_to_vector(str_without_name, t_c, '>');
            t_c.add_bus(name, stops, true);
            continue;
        }

        pos_comm = str_query.text.find('-');
        if(pos_comm != std::string::npos) {
            std::string str_without_name = str_query.text.substr(pos+2);
            std::vector<Stop*> stops = read_route_to_vector(str_without_name, t_c, '-');
            t_c.add_bus(name, stops, false);
            continue;
        }
    }

    for(const auto [key,value] : dist_query_queue) {
        string_to_distance(key, value.first, value.second, t_c);
    }
}

std::vector<Stop*> t_c_i_r::read_route_to_vector(std::string& str, TransportCatalogue& t_c, char ch) {
    std::vector<Stop*> route;
    auto pos_comm = str.find_first_of(ch);
    size_t pos = 0;

    while(pos_comm != std::string::npos) {
        route.push_back(t_c.pointer_stop_name(str.substr(pos, pos_comm-pos-1)));
        pos = pos_comm + 2;
        pos_comm = str.find_first_of(ch, pos);
    }
    route.push_back(t_c.pointer_stop_name(str.substr(pos)));
    return route;
}

void t_c_i_r::string_to_distance(std::string_view name, Query* query, size_t pos, TransportCatalogue& t_c) {
    auto pos1 = pos;
    auto pos2 = query->text.find("m ",pos1);
    while(pos2 != std::string::npos) {
        int distance = std::stoi(std::string(query->text.substr(pos1, pos2 - pos1)));
        pos1 = query->text.find(" to ", pos2);
        pos2 = query->text.find(',', pos1);
        //t_c.add_distance_to_stop()
        std::string neib_stop_name = query->text.substr(pos1 + 4, pos2 - pos1 -4);
        t_c.add_distance_to_stop(name, neib_stop_name, distance);

        if(pos2 == std::string::npos) break;
        pos1 = pos2 + 1;
        pos2 = query->text.find("m ", pos1);
    }
}

const std::set<Bus*, bus_compare>& b_t_p::all_buses_from_tc(TransportCatalogue& t_c) {
    return t_c.all_buses();
}

void print_map::print_map(TransportCatalogue& t_c, RenderSettings& r_s, svg::Document& svg_doc, std::ostream& out){
     const std::set<Bus*, bus_compare>& buses = b_t_p::all_buses_from_tc(t_c);
     SphereProjector proj = print_coordinates(buses, r_s);
     print_route_line(buses, proj, r_s, svg_doc);
     print_bus_name(buses, proj, r_s, svg_doc);
     print_stop_circle(buses, proj, r_s, svg_doc);
     print_stop_name(buses, proj, r_s, svg_doc);

     svg_doc.Render(out);
}
