#pragma once
#include "request_handler.h"
#include "json.h"
#include "map_renderer.h"
/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */
json::Document read_json(std::istream& input_stream);

void json_to_tc(json::Document& json_d, TransportCatalogue& t_c);

std::vector<json::Node> json_stat_from_tc(json::Document& json_d, TransportCatalogue& t_c, RenderSettings& r_s);
void json_to_rs(json::Document& json_d, RenderSettings& r_s);

json::Dict error_dict(int id);

const std::string err_msg = "not found";
std::string& svg_to_json_format(std::string& text);
