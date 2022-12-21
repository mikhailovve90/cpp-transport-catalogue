#pragma once
#include "json.h"
#include "map_renderer.h"
/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */

namespace json_reader {
class JSONReader {
public:
    JSONReader(std::istream& input_stream) : doc_being_proc(read_json(input_stream)) {}
    json::Document read_json(std::istream& input_stream);
    JSONReader& parse_transport_catalogue(TransportCatalogue& t_c);
    JSONReader& parse_render_settings(RenderSettings& r_s);
    //std::vector<json::Node> json_stat_from_tc(std::vector<json::Node>& stat_requests, TransportCatalogue& t_c, RenderSettings& r_s);
    json::Node stop_req_processing(const std::map<std::string, json::Node>& map_node, const TransportCatalogue& t_c) const;
    json::Node bus_req_processing(const std::map<std::string, json::Node>& map_node, const BusInfo& b_i) const;
    json::Node svg_req_processing(const std::map<std::string, json::Node>& map_node, const std::string& svg_str) const;
    json::Dict error_dict(int id) const;
    std::string& svg_to_json_format(std::string& text);
    void set_color_pallete(const std::vector<json::Node>& c_p_array, RenderSettings& r_s);
    svg::Color set_rgb_rgba_color(const std::vector<json::Node>& c_array);
    json::Document& get_doc() {return doc_being_proc;}
private:
    json::Document doc_being_proc;
    const std::string err_msg = "not found";

};
}
