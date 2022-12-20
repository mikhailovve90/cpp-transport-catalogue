#include "json_reader.h"
#include "request_handler.h"

int main() {
     TransportCatalogue t_c;
     RenderSettings r_s;
     json_reader::JSONReader v(std::cin);
     v.parse_transport_catalogue(t_c).parse_render_settings(r_s);
     MapRenderer m_r(t_c, r_s);
     RequestHandler r_h(t_c, m_r, v);
     json::render_value(r_h.processing_requests(), std::cout);
}
