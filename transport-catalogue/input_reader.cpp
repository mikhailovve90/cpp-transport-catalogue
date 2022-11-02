#include "input_reader.h"

// напишите решение с нуля
// код сохраните в свой git-репозиторий

std::string t_c_i_r::read_line(std::istream& input_stream)
{
    std::string s;
    getline(input_stream, s);
    return s;
}

int t_c_i_r::read_line_with_number(std::istream& input_stream)
{
    int result = std::stoi(t_c_i_r::read_line(input_stream));

    return result;
}

t_c_i_r::Query t_c_i_r::string_to_query(std::string& str_query)
{
    t_c_i_r::Query result;
    int pos = str_query.find(' ');
    std::string sub_str = str_query.substr(0, pos);
    if(sub_str == "Bus")
    {
        result.type = t_c_i_r::QueryType::BUS;
    }
    else if(sub_str == "Stop")
    {
        result.type = t_c_i_r::QueryType::STOP;
    }
    result.text = std::move(str_query.substr(pos + 1));
    return result;
}

std::unordered_map<t_c_i_r::QueryType, std::deque<t_c_i_r::Query>> t_c_i_r::read_query_queue(std::istream& input_stream)
{
    std::unordered_map<t_c_i_r::QueryType, std::deque<t_c_i_r::Query>> result;
    int count_query = t_c_i_r::read_line_with_number(input_stream);
    for(int i = 0; i < count_query; ++i)
    {
        std::string str = t_c_i_r::read_line(input_stream);
        auto query = t_c_i_r::string_to_query(str);
        result[query.type].push_back(query);
    }
    return result;
}

//Обработка запросов на добавление остановок
void t_c_i_r::processing_query_queue(std::unordered_map<QueryType, std::deque<Query>>& q_q, TransportCatalogue& t_c)
{
    std::unordered_map<std::string_view, std::pair<Query*, long >> dist_query_queue;
    size_t counter = 0;
    for(const t_c_i_r::Query& str_query : q_q[t_c_i_r::QueryType::STOP])//иду по запросам на добавление остановок
    {
        int pos = str_query.text.find(':');
        std::string name = std::move(str_query.text.substr(0 , pos));

        int pos_comm = str_query.text.find(',');
        double latitude = std::stod(str_query.text.substr(pos+1, pos_comm));//из строки выцепляю широту
        pos = str_query.text.find(',', pos_comm + 1);
        double longitude = std::stod(str_query.text.substr(pos_comm+1, pos - pos_comm));//из строки выцепляю долготу
        t_c.add_stop(name, latitude, longitude);
        dist_query_queue[t_c.pointer_stop_name(name)->name] = std::pair(&q_q[t_c_i_r::QueryType::STOP][counter], pos + 1);
        ++counter;
    }


    for(const t_c_i_r::Query& str_query : q_q[t_c_i_r::QueryType::BUS])//иду по запросам на добавление остановок
    {
        std::vector<TransportCatalogue::Stop*> route;
        auto pos = str_query.text.find(':');
        std::string name = std::move(str_query.text.substr(0 , pos));

        auto pos_comm = str_query.text.find('>');
        if(pos_comm != std::string::npos)
        {
            std::string str_without_name = str_query.text.substr(pos+2);
            std::vector<TransportCatalogue::Stop*> stops = read_route_to_vector(str_without_name, t_c, '>');
            t_c.add_bus(name, stops, true);
            continue;
        }

        pos_comm = str_query.text.find('-');
        if(pos_comm != std::string::npos)
        {
            std::string str_without_name = str_query.text.substr(pos+2);
            std::vector<TransportCatalogue::Stop*> stops = read_route_to_vector(str_without_name, t_c, '-');
            t_c.add_bus(name, stops, false);
            continue;
        }
    }

    for(const auto [key ,value] : dist_query_queue)
    {
        //std::cout << value.first->text << std::endl;
        string_to_distance(key, value.first, value.second, t_c);
    }
}

std::vector<TransportCatalogue::Stop*> t_c_i_r::read_route_to_vector(std::string& str, TransportCatalogue& t_c, char ch)
{
    std::vector<TransportCatalogue::Stop*> route;
    auto pos_comm = str.find_first_of(ch);
    size_t pos = 0;

    while(pos_comm != std::string::npos)
    {
        route.push_back(t_c.pointer_stop_name(str.substr(pos, pos_comm-pos-1)));
        pos = pos_comm + 2;
        pos_comm = str.find_first_of(ch, pos);
    }
    route.push_back(t_c.pointer_stop_name(str.substr(pos)));
    return route;
}

void t_c_i_r::string_to_distance(std::string_view name, Query* query, size_t pos, TransportCatalogue& t_c)
{
    auto pos1 = pos;
    auto pos2 = query->text.find("m ",pos1);
    while(pos2 != std::string::npos)
    {
        int distance = std::stoi(std::string(query->text.substr(pos1, pos2 - pos1)));
        pos1 = query->text.find(" to ", pos2);
        pos2 = query->text.find(',', pos1);
        //t_c.add_distance_to_stop()
        std::string neib_stop_name = query->text.substr(pos1 + 4, pos2 - pos1 -4);
        //std::cout << "name" << name << "name2" << query->text.substr(pos1 + 4 , pos2 - pos1 - 4) << "NAME" << distance << std::endl;
        //std::cout << "Name " << neib_stop_name << std::endl;
        t_c.add_distance_to_stop(name, neib_stop_name, distance);

        if(pos2 == std::string::npos) break;
        pos1 = pos2 + 1;
        pos2 = query->text.find("m ", pos1);
    }
}
