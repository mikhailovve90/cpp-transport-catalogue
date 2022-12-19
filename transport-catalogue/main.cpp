#include "json_reader.h"

int main() {
     TransportCatalogue t_c;
     RenderSettings r_s;
     json::Document doc = read_json(std::cin);
     JsonProcessing v(doc);
     v.json_to_tc(t_c).json_to_rs(r_s);
     MapRender m_r(t_c, r_s);
     RequestHandler r_h(t_c, m_r);

     const auto stat_req = v.json_stat_from_tc(r_h);
     json::PrintValue(stat_req, std::cout);
}
