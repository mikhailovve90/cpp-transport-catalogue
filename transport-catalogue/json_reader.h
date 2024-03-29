#pragma once
#include "json.h"
#include "map_renderer.h"
#include "transport_router.h"
#include "serialization.h"
/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */

namespace json_reader {
class JSONReader {
  public:
    JSONReader(std::istream& input_stream) : doc_being_processed(read_json(input_stream)) {}
    json::Document read_json(std::istream& input_stream);
    JSONReader& parse_transport_catalogue(TransportCatalogue& t_c);
    JSONReader& parse_routing_settings(TransportRouter& t_c);
    JSONReader& parse_render_settings(RenderSettings& r_s);
    JSONReader& parse_serialize_settings(SerializationTC& s_t_c);
    //std::vector<json::Node> json_stat_from_tc(std::vector<json::Node>& stat_requests, TransportCatalogue& t_c, RenderSettings& r_s);
    json::Node stop_req_processing(const std::map<std::string, json::Node>& map_node, const TransportCatalogue& t_c) const;
    json::Node bus_req_processing(const std::map<std::string, json::Node>& map_node, const BusInfo& b_i) const;
    json::Node svg_req_processing(const std::map<std::string, json::Node>& map_node, const std::string& svg_str) const;
    json::Node route_req_processing(const std::map<std::string, json::Node>& map_node, const TransportRouter& t_r, std::optional<graph::Router<double>::RouteInfo> route_info) const;
    json::Dict error_dict(int id) const;
    std::string& svg_to_json_format(std::string& text);
    void set_color_pallete(const std::vector<json::Node>& c_p_array, RenderSettings& r_s);
    svg::Color set_rgb_rgba_color(const std::vector<json::Node>& c_array);
    json::Document& get_doc() {
        return doc_being_processed;
    }

  private:
    json::Document doc_being_processed;
    const std::string err_msg = "not found";

};
}
