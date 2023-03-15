#pragma once
#include <transport_catalogue.pb.h>
#include <svg.pb.h>
#include <map_renderer.pb.h>
#include "transport_catalogue.h"
#include "transport_router.pb.h"
#include "graph.pb.h"
#include <filesystem>
#include <fstream>
#include <ios>
#include <vector>
#include "map_renderer.h"
#include "transport_router.h"

using Path = std::filesystem::path;

class SerializationTC {
public:
    void set_file_serialization_name(const std::string& name);

    const std::string& get_file_serialization();

    void serialize();
    void serialize_tc(TransportCatalogue& t_c);
    void serialize_rs(RenderSettings& r_s);
    void serialize_tr(TransportRouter& t_r);

    TransportCatalogue deserialize_tc();
    RenderSettings deserialize_rs();
    void deserialize_tr(TransportRouter& t_r);
    void deserialize();
private:
    std::string file_for_serialization;
    tc_serialize::Database db;
};



