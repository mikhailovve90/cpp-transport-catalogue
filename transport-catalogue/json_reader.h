#pragma once
#include "request_handler.h"
#include "json.h"
#include "map_renderer.h"
/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */
json::Document read_json(std::istream& input_stream);

class JsonProcessing {
public:
    JsonProcessing(json::Document& doc) : doc_being_proc(doc){};

    JsonProcessing& json_to_tc(TransportCatalogue& t_c);
    JsonProcessing& json_to_rs(RenderSettings& r_s);
    std::vector<json::Node> json_stat_from_tc(RequestHandler& r_h);


    json::Dict error_dict(int id);
    std::string& svg_to_json_format(std::string& text);
    void set_color_pallete(const std::vector<json::Node>& c_p_array, RenderSettings& r_s);
    svg::Color set_rgb_rgba_color(const std::vector<json::Node>& c_array);
private:
    json::Document& doc_being_proc;
    const std::string err_msg = "not found";
};

