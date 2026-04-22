#ifndef OSRM_UTIL_PO_FILESYSTEM_HPP
#define OSRM_UTIL_PO_FILESYSTEM_HPP

// Provide Boost.Program_Options validate() for std::filesystem::path.
//
// Boost.PO's generic validate template falls back to boost::lexical_cast<T>(string),
// which throws bad_lexical_cast for std::filesystem::path because the type has no
// compatible stream operator.  The fix is a validate() overload in the std::filesystem
// namespace so ADL finds it when the target type is std::filesystem::path*.

#include <boost/any.hpp>
#include <boost/program_options/errors.hpp>
#include <boost/program_options/validators.hpp>
#include <filesystem>
#include <string>
#include <vector>

namespace std::filesystem
{
inline void validate(boost::any &v,
                     const std::vector<std::string> &values,
                     path * /* target_type */,
                     int /* unused */)
{
    boost::program_options::validators::check_first_occurrence(v);
    const std::string &s = boost::program_options::validators::get_single_string(values);
    v = path(s);
}
} // namespace std::filesystem

#endif // OSRM_UTIL_PO_FILESYSTEM_HPP
