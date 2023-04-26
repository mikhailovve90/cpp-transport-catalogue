Transport_catalogue

Creates a route database, receiving as input a list of stops, a list of distances between stops and a list of routes (json). Binary serializes via protobuf. Deserializes the database and processes requests to the database. Print routes and stops info. Calculates the shortest route between stops. Finding distance through graph (DirectedWeightedGraph) and router. Builds route map as svg image.

C++17 required CMake min3.10 required
