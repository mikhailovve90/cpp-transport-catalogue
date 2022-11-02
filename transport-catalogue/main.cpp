#include <iostream>
#include "input_reader.h"
#include "transport_catalogue.h"
#include "stat_reader.h"
#include <cassert>

using namespace std;
/*
void test_input_reader(){
    std::istream a;
    std::unordered_map<t_c_i_r::QueryType, std::deque<t_c_i_r::Query>> test_query = t_c_i_r::read_query_queue(a);
    assert(test_query[t_c_i_r::QueryType::STOP].size() == 8);
    assert(test_query[t_c_i_r::QueryType::STOP][0].text ==  "Tolstopaltsevo: 55.611087, 37.208290");
    assert(test_query[t_c_i_r::QueryType::STOP][1].text ==  "Marushkino: 55.595884, 37.209755");
    assert(test_query[t_c_i_r::QueryType::STOP][2].text ==  "Rasskazovka: 55.632761, 37.333324");
    assert(test_query[t_c_i_r::QueryType::STOP][3].text ==  "Biryulyovo Zapadnoye: 55.574371, 37.651700");
    assert(test_query[t_c_i_r::QueryType::STOP][4].text ==  "Biryusinka: 55.581065, 37.648390");
    assert(test_query[t_c_i_r::QueryType::STOP][5].text ==  "Universam: 55.587655, 37.645687");
    assert(test_query[t_c_i_r::QueryType::STOP][6].text ==  "Biryulyovo Tovarnaya: 55.592028, 37.653656");
    assert(test_query[t_c_i_r::QueryType::STOP][7].text ==  "Biryulyovo Passazhirskaya: 55.580999, 37.659164");
    assert(test_query[t_c_i_r::QueryType::BUS][0].text ==  "256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye");
    assert(test_query[t_c_i_r::QueryType::BUS][1].text ==  "750: Tolstopaltsevo - Marushkino - Rasskazovka");
}

void test_add_stop_and_bus(){
    std::unordered_map<t_c_i_r::QueryType, std::deque<t_c_i_r::Query>> test_query = t_c_i_r::read_query_queue();
    TransportCatalogue t_c;
    t_c_i_r::processing_query_queue(test_query, t_c);
    auto& t_c_stops = t_c.stops_return();
    assert(t_c_stops[0].name == "Tolstopaltsevo");
    assert(t_c_stops[0].latitude - 55.611087 < std::abs(0.00000001));
    assert(t_c_stops[0].longitude - 37.208290 < std::abs(0.00000001));
    assert(t_c.pointer_stop_name("Tolstopaltsevo") == &t_c_stops[0]);
    assert(t_c_stops[1].name == "Marushkino");
    assert(t_c_stops[1].latitude - 55.595884 < std::abs(0.00000001));
    assert(t_c_stops[1].longitude - 37.209755 < std::abs(0.00000001));
    assert(t_c.pointer_stop_name("Marushkino") == &t_c_stops[1]);//

    auto& t_c_buses = t_c.buses_return();
    assert(t_c_buses[0].name_ == "256");
    assert(t_c_buses[1].name_ == "750");

    assert(t_c_buses[0].its_ring() == true);
    assert(t_c_buses[1].its_ring() == false);
    //std::cout << t_c_buses[0].route[4] << std::endl;
    assert(t_c_buses[0].route_[0] == &t_c_stops[3]);
    assert(t_c_buses[0].route_[1] == t_c.pointer_stop_name("Biryusinka"));
    assert(t_c_buses[0].route_[5] == &t_c_stops[3]);

    assert(t_c_buses[1].route_[0] == t_c.pointer_stop_name("Tolstopaltsevo"));

}

void test_stop_distance(){
    std::unordered_map<t_c_i_r::QueryType, std::deque<t_c_i_r::Query>> test_query = t_c_i_r::read_query_queue();
    TransportCatalogue t_c;
    t_c_i_r::processing_query_queue(test_query, t_c);
    std::string n1 = "Biryulyovo Tovarnaya";
    std::string n2 = "Biryulyovo Passazhirskaya";
    long r = t_c.distancse_2stops(n1, n2);
    assert(r == 1300);
    n1 = "Biryulyovo Zapadnoye";
    n2 = "Biryusinka";
    r = t_c.distancse_2stops(n1, n2);
    assert(r == 1800);


}*/


int main()
{
    TransportCatalogue t_c;
    std::unordered_map<t_c_i_r::QueryType, std::deque<t_c_i_r::Query>> test_query = t_c_i_r::read_query_queue(std::cin);
    t_c_i_r::processing_query_queue(test_query, t_c);
    t_c_o_r::read_processing_queue(t_c, std::cin, std::cout);
    return 0;
}

/*
10
Stop Tolstopaltsevo: 55.611087, 37.208290
Stop Marushkino: 55.595884, 37.209755
Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye
Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka
Stop Rasskazovka: 55.632761, 37.333324
Stop Biryulyovo Zapadnoye: 55.574371, 37.651700
Stop Biryusinka: 55.581065, 37.648390
Stop Universam: 55.587655, 37.645687
Stop Biryulyovo Tovarnaya: 55.592028, 37.653656
Stop Biryulyovo Passazhirskaya: 55.580999, 37.659164
3
Bus 256
Bus 750
Bus 751*/
