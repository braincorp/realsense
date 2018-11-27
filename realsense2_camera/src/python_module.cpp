#include <pybind11/pybind11.h>
#include "base_realsense_node.h"

PYBIND11_MODULE(test_base_realsense_node_module, m) {
    m.doc() = "Test suite for the RealSense ROS Node";
    pybind11::class_<BaseRealSenseNode>(m, "BaseRealSenseNode")
        .def_static("calculateDepthVector", &BaseRealSenseNode::calculateDepthVector);
}

