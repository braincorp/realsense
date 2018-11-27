#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>
#include <pybind11/pybind11.h>

// Per Catch's documentation, they recommend placing test cases and the catch entry
// point in separate files to speed up compilation.
// https://github.com/catchorg/Catch2/blob/master/docs/slow-compiles.md

/// Entrypoint from python to start testing.
int startTest( std::vector<std::string> args = std::vector<std::string>()) {
    // Here we are basically simulating the C main() fn, 
    std::vector<char *> argv; argv.reserve(args.size() + 1);
    std::string cmd("test_voxel_raytrace_cuda");
    argv.push_back(const_cast<char*>(cmd.c_str()));
    for(auto & anArg: args) argv.push_back(const_cast<char*>(anArg.c_str()));
    int result = Catch::Session().run(argv.size(), argv.data());

    return result;
}

//-----------------------------------------------------------------------------

PYBIND11_MODULE(test_base_realsense_node_module, m) {
    m.doc() = "Test suite for the RealSense ROS Node";
}

