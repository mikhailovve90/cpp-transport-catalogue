#pragma once
#include <string>
#include <iostream>
#include <deque>
#include <unordered_map>
#include "transport_catalogue.h"
#include <vector>
// напишите решение с нуля
// код сохраните в свой git-репозиторий
namespace t_c_i_r
{

enum class QueryType
{
    STOP,
    BUS
};


struct Query
{
    std::string text;
    QueryType type;
};

std::string read_line();
int read_line_with_number();

std::unordered_map<QueryType, std::deque<Query>> read_query_queue();

t_c_i_r::Query string_to_query(std::string& str_query);

void processing_query_queue(std::unordered_map<QueryType, std::deque<Query>>& q_q, TransportCatalogue& t_c);

std::vector<TransportCatalogue::Stop*> read_route_to_vector(std::string& str, TransportCatalogue& t_c, char ch);
void string_to_distance(std::string_view name, Query* query, size_t pos, TransportCatalogue& t_c);

}








