#include "stat_reader.h"

// напишите решение с нуля
// код сохраните в свой git-репозиторий
void t_c_o_r::read_processing_queue(TransportCatalogue& t_c)
{
    int count_query = t_c_i_r::read_line_with_number();
    for(int i = 0; i < count_query; ++i)
    {
        std::string str = t_c_i_r::read_line();
        t_c_i_r::Query query = t_c_i_r::string_to_query(str);
        std::string name = t_c_o_r::bstrip(query.text.substr(query.text.find_first_not_of(' ')));
        if(query.type == t_c_i_r::QueryType::BUS)
        {
            if(t_c.pointer_bus_name(name) != nullptr)
            {
                bus_information(t_c.pointer_bus_name(name), t_c);
            } else
            {
                std::cout << "Bus " << name << ": not found" << std::endl;
            }
        } else if(query.type == t_c_i_r::QueryType::STOP)
        {
            if(t_c.pointer_stop_name(name) == nullptr)
            {
                std::cout << "Stop " << name << ": not found" << std::endl;;

            } else if(t_c.stopname_to_buses(name).empty())
            {
                std::cout << "Stop " << name << ": no buses" << std::endl;
            } else
            {
                std::cout << "Stop " << name << ": buses";
                stop_information(t_c.stopname_to_buses(name));
            }
        }
    }
}

void t_c_o_r::bus_information(TransportCatalogue::Bus* bus, TransportCatalogue& t_c)
{

    long bus_route_lenght = t_c.calculate_distance(bus);

    double bus_route_lenght_geo = 0;
    for(size_t t = 0; t < bus->route_.size() - 1 ; ++t)
    {
        bus_route_lenght_geo += ComputeDistance({bus->route_[t]->latitude, bus->route_[t]->longitude}, {bus->route_[t+1]->latitude, bus->route_[t+1]->longitude});
    }
    if(!bus->its_ring())
    {
        bus_route_lenght_geo *= 2;
    }

    size_t route_size = bus->its_ring()?bus->route_size():bus->route_size()*2 - 1;

    auto copy_route = bus->route_;
    size_t uniq_stops_count;
    std::sort(copy_route.begin(), copy_route.end(),
         [](const TransportCatalogue::Stop* a,const TransportCatalogue::Stop* b){ return std::lexicographical_compare(a->name.begin(), a->name.end(), b->name.begin(), b->name.end()); });
         uniq_stops_count = distance(copy_route.begin(),std::unique(copy_route.begin(),copy_route.end()));

    std::cout << std::setprecision(6) << "Bus " << bus->name_ << ": " << route_size << " stops on route, "
    << uniq_stops_count << " unique stops, "
    << static_cast<double>(bus_route_lenght) << " route length"
    <<  ", " << bus_route_lenght/bus_route_lenght_geo << " curvature" << std::endl;
}

void t_c_o_r::stop_information(const std::set<TransportCatalogue::Bus*, TransportCatalogue::bus_compare>& buses)
{
    for(const auto bus : buses)
    {
        std::cout << " " << bus->name_;
    }
    std::cout << std::endl;
}

std::string t_c_o_r::bstrip(std::string line)
{
    while (!line.empty() && isspace(line[line.size()-1]))
    {
        line.pop_back();
    }
    return line;
}


