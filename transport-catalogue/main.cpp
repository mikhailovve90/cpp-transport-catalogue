#include "json_reader.h"
#include "request_handler.h"
using namespace std::literals;
int main() {
     TransportCatalogue t_c;
     RenderSettings r_s;
     json_reader::JSONReader v(std::cin);
     v.parse_transport_catalogue(t_c).parse_render_settings(r_s);
     MapRenderer m_r(t_c, r_s);
     RequestHandler r_h(t_c, m_r, v);
     r_h.process_requests();
     json::Print(json::Document(r_h.building_node()), std::cout);

     json::Print(
        json::Document{
            json::Builder{}
            .StartDict()
                .Key("key1"s).Value(123)
                .Key("key2"s).Value("value2"s)
                .Key("key3"s).StartArray()
                    .Value(456)
                    .StartDict().EndDict()
                    .StartDict()
                        .Key(""s)
                        .Value(nullptr)
                    .EndDict()
                    .Value(""s)
                .EndArray()
            .EndDict()
            .Build()
        },
        std::cout
    );
    std::cout << std::endl;

   json::Print(
        json::Document{
            json::Builder{}
            .Value("just a string"s)
            .Build()
        },
        std::cout
    );
    std::cout << std::endl;

//    json::Builder{}.StartDict().Build();  // правило 3
//    json::Builder{}.StartDict().Key("1"s).Value(1).Value(1);  // правило 2
//    json::Builder{}.StartDict().Key("1"s).Key(""s);  // правило 1
//    json::Builder{}.StartArray().Key("1"s);  // правило 4
//    json::Builder{}.StartArray().EndDict();  // правило 4
//    json::Builder{}.StartArray().Value(1).Value(2).EndDict();  // правило 5
}
