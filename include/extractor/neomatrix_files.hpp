#ifndef OSRM_EXTRACTOR_NEOMATRIX_FILES_HPP
#define OSRM_EXTRACTOR_NEOMATRIX_FILES_HPP

// [Neomatrix fork] File I/O for the v5 legacy OSRM export.
// In v5, the osrm output a .osrm file which had the raw graph, paths, edges, annotations, traffic lights, and obstacles.
// In v6, this was removed. We have replaced it with a similar export .osrm.v5-legacy-osrm.
// Writes/reads the raw (pre-compression) node-based graph with annotations and traffic lights,
// which NeomatrixCore's NodeBasedGraph consumes directly.
// Note that obstacles have not been updated into this legacy file as they are not currently used in Neomatrix.

#include "extractor/node_based_edge.hpp"
#include "extractor/query_node.hpp"
#include "extractor/serialization.hpp"

#include "util/coordinate.hpp"
#include "util/packed_vector.hpp"

#include <boost/iterator/function_output_iterator.hpp>
#include <vector>

namespace osrm::extractor::neomatrix_files
{

// Read raw node-based graph with annotation data and traffic lights.
template <typename PackedOSMIDsT>
void readRawNBGraph(const std::filesystem::path &path,
                    std::vector<util::Coordinate> &coordinates,
                    PackedOSMIDsT &osm_node_ids,
                    std::vector<extractor::NodeBasedEdge> &edge_list,
                    std::vector<extractor::NodeBasedEdgeAnnotation> &annotations,
                    std::vector<NodeID> &traffic_lights)
{
    const auto fingerprint = storage::tar::FileReader::VerifyFingerprint;
    storage::tar::FileReader reader{path, fingerprint};

    auto number_of_nodes = reader.ReadElementCount64("/extractor/nodes");
    coordinates.resize(number_of_nodes);
    osm_node_ids.reserve(number_of_nodes);
    auto index = 0;
    auto decode = [&](const auto &current_node)
    {
        coordinates[index].lon = current_node.lon;
        coordinates[index].lat = current_node.lat;
        osm_node_ids.push_back(current_node.node_id);
        index++;
    };
    reader.ReadStreaming<extractor::QueryNode>("/extractor/nodes",
                                               boost::make_function_output_iterator(decode));

    storage::serialization::read(reader, "/extractor/edges", edge_list);
    storage::serialization::read(reader, "/extractor/annotations", annotations);
    storage::serialization::read(reader, "/extractor/traffic_lights", traffic_lights);
}

// Write raw (uncompressed) node-based graph with full edge data.
// Produces a tar with /extractor/nodes (QueryNode), /extractor/edges (NodeBasedEdge),
// /extractor/annotations, and /extractor/traffic_lights.
// Used by NeomatrixCore's NodeBasedGraph for direct graph access.
template <typename CoordinatesT, typename PackedOSMIDsT>
void writeRawNBGraph(const std::filesystem::path &path,
                     const CoordinatesT &coordinates,
                     const PackedOSMIDsT &osm_node_ids,
                     const std::vector<extractor::NodeBasedEdge> &edge_list,
                     const std::vector<extractor::NodeBasedEdgeAnnotation> &annotations,
                     const std::vector<NodeID> &traffic_lights)
{
    const auto fingerprint = storage::tar::FileWriter::GenerateFingerprint;
    storage::tar::FileWriter writer{path, fingerprint};

    std::vector<QueryNode> nodes(coordinates.size());
    for (std::size_t i = 0; i < coordinates.size(); ++i)
    {
        nodes[i] = QueryNode(coordinates[i].lon, coordinates[i].lat, osm_node_ids[i]);
    }
    storage::serialization::write(writer, "/extractor/nodes", nodes);
    storage::serialization::write(writer, "/extractor/edges", edge_list);
    storage::serialization::write(writer, "/extractor/annotations", annotations);
    storage::serialization::write(writer, "/extractor/traffic_lights", traffic_lights);
}

} // namespace osrm::extractor::neomatrix_files

#endif
