#include "json_reader.h"

int main() {
    /*
     * Примерная структура программы:
     *
     * Считать JSON из stdin
     * Построить на его основе JSON базу данных транспортного справочника
     * Выполнить запросы к справочнику, находящиеся в массиве "stat_requests", построив JSON-массив
     * с ответами.
     * Вывести в stdout ответы в виде JSON
     */
     TransportCatalogue t_c;
     RenderSettings r_s;
     //svg::Document svg_doc;

     json::Document doc = read_json(std::cin);
     json_to_tc(doc,t_c);
     json_to_rs(doc, r_s);
     const auto stat_req = json_stat_from_tc(doc,t_c, r_s);
     json::PrintValue(stat_req, std::cout);
     //print_map::print_map(t_c, r_s, svg_doc, std::cout);
}
