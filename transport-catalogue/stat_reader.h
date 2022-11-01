#pragma once
#include "input_reader.h"
#include "transport_catalogue.h"
#include "geo.h"
#include <numeric>
#include <algorithm>
#include <deque>
#include<iomanip>

// напишите решение с нуля
// код сохраните в свой git-репозиторий
namespace t_c_o_r
{
void read_processing_queue(TransportCatalogue& t_c);

void bus_information(TransportCatalogue::Bus* bus, TransportCatalogue& t_c);

void stop_information(const std::set<TransportCatalogue::Bus*, TransportCatalogue::bus_compare>& buses);

std::string bstrip(std::string line);
}
