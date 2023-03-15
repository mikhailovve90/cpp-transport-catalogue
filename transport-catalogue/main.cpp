#include <fstream>
#include <iostream>
#include <string_view>
#include "serialization.h"
#include "transport_catalogue.h"
#include "json_reader.h"
#include "request_handler.h"
using namespace std::literals;

void PrintUsage(std::ostream& stream = std::cerr) {
    stream << "Usage: transport_catalogue [make_base|process_requests]\n"sv;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        PrintUsage();
        return 1;
    }

    const std::string_view mode(argv[1]);

    if (mode == "make_base"sv) {
            SerializationTC s_t_c;
            TransportCatalogue t_c;
            RenderSettings r_s;

            json_reader::JSONReader v(std::cin);
            v.parse_transport_catalogue(t_c).parse_render_settings(r_s).parse_serialize_settings(s_t_c);//parse_routing_settings(t_c);
            MapRenderer m_r(t_c, r_s);
            RequestHandler r_h(t_c, m_r, v, true);

            s_t_c.serialize_tc(t_c);
            s_t_c.serialize_rs(r_s);
            s_t_c.serialize_tr(r_h.get_tansport_router());
            s_t_c.serialize();

    } else if (mode == "process_requests"sv) {
        TransportCatalogue t_c;
        SerializationTC s_t_c;
        RenderSettings r_s;

        json_reader::JSONReader v(std::cin);
        v.parse_serialize_settings(s_t_c);
        s_t_c.deserialize();
        t_c = s_t_c.deserialize_tc();
        r_s = s_t_c.deserialize_rs();

        MapRenderer m_r(t_c, r_s);
        RequestHandler r_h(t_c, m_r, v, false);
        s_t_c.deserialize_tr(r_h.get_tansport_router());
        r_h.reinitialize_router();
        r_h.process_requests();
        json::Print(json::Document(r_h.building_node()), std::cout);

    } else {
        PrintUsage();
        return 1;
    }
}
