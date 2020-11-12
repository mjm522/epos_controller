#include <boost/python.hpp> 
#include "epos_lib.cpp"

using namespace boost::python;

BOOST_PYTHON_MODULE(epos_boost)
{
    class_<EPOS4Boost>("EPOS4Boost")
        .def("open_device", &EPOS4Boost::OpenDevice)
        .def("set_rls_econder", &EPOS4Boost::setupRLSEncoder)
        .def("get_rls_econder", &EPOS4Boost::checkRLSEncoderSettings)
        .def("close_device", &EPOS4Boost::CloseDevice)
        .def("min_position", &EPOS4Boost::setMinPosition)
        .def("set_target_position_profile", &EPOS4Boost::PositionProfileMode)
        .def("set_target_velocity_profile", &EPOS4Boost::VelocityProfileMode)
    ;
}
