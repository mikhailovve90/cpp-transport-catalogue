/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 *
 * В качестве источника для идей предлагаем взглянуть на нашу версию обработчика запросов.
 * Вы можете реализовать обработку запросов способом, который удобнее вам.
 *
 * Если вы затрудняетесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
 */

// Класс RequestHandler играет роль Фасада, упрощающего взаимодействие JSON reader-а
// с другими подсистемами приложения.
// См. паттерн проектирования Фасад: https://ru.wikipedia.org/wiki/Фасад_(шаблон_проектирования)
/*
class RequestHandler {
public:
    // MapRenderer понадобится в следующей части итогового проекта
    RequestHandler(const TransportCatalogue& db, const renderer::MapRenderer& renderer);

    // Возвращает информацию о маршруте (запрос Bus)
    std::optional<BusStat> GetBusStat(const std::string_view& bus_name) const;

    // Возвращает маршруты, проходящие через
    const std::unordered_set<BusPtr>* GetBusesByStop(const std::string_view& stop_name) const;

    // Этот метод будет нужен в следующей части итогового проекта
    svg::Document RenderMap() const;

private:
    // RequestHandler использует агрегацию объектов "Транспортный Справочник" и "Визуализатор Карты"
    const TransportCatalogue& db_;
    const renderer::MapRenderer& renderer_;
};
*/
#pragma once
#include <string>
#include <iostream>
#include <deque>
#include <unordered_map>
#include "transport_catalogue.h"
#include "geo.h"
#include <vector>
#include <numeric>
#include <algorithm>
#include <iomanip>
#include "map_renderer.h"
// напишите решение с нуля
// код сохраните в свой git-репозиторий
namespace t_c_i_r {

enum class QueryType {
    STOP,
    BUS
};


struct Query {
    std::string text;
    QueryType type;
};

std::string read_line(std::istream& input_stream);
int read_line_with_number(std::istream& input_stream);

std::unordered_map<QueryType, std::deque<Query>> read_query_queue(std::istream& input_stream);

t_c_i_r::Query string_to_query(std::string& str_query);

void processing_query_queue(std::unordered_map<QueryType, std::deque<Query>>& q_q, TransportCatalogue& t_c);

std::vector<Stop*> read_route_to_vector(std::string& str, TransportCatalogue& t_c, char ch);
void string_to_distance(std::string_view name, Query* query, size_t pos, TransportCatalogue& t_c);
}

// напишите решение с нуля
// код сохраните в свой git-репозиторий
namespace t_c_o_r {
void read_processing_queue(TransportCatalogue& t_c,std::istream& input_stream, std::ostream& output_stream);

void bus_information(Bus* bus, TransportCatalogue& t_c, std::ostream& output_stream);

void stop_information(const std::set<Bus*, bus_compare>& buses, std::ostream& output_stream);

BusInfo get_bus_info(Bus* bus, TransportCatalogue& t_c);

std::string bstrip(std::string line);
}

namespace b_t_p {

const std::set<Bus*, bus_compare>& all_buses_from_tc(TransportCatalogue& t_c);

}

namespace print_map {
    void print_map(TransportCatalogue& t_c, RenderSettings& r_s, svg::Document& svg_doc, std::ostream& out);
}

