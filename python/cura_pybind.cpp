#include <pybind11/pybind11.h>
#include "../src/utils/floatpoint.h"
#include "../src/utils/FMatrix4x3.h"
#include "../src/utils/Point3.h"

#include <pybind11/stl.h>


namespace py = pybind11;

PYBIND11_MODULE(cura, m) {
    m.doc() = "Cura Engine Python Bindings";

    py::class_<cura::FPoint3>(m, "FPoint")
            .def(py::init<float, float, float>(),  py::arg("x"), py::arg("y"),  py::arg("z"))
            .def_property_readonly("x", &cura::FPoint3::x)
            .def_property_readonly("y", &cura::FPoint3::y)
            .def_property_readonly("z", &cura::FPoint3::z)
            ;
    
    py::class_<cura::Point3>(m, "Point")
            .def(py::init<int, int, int>(),  py::arg("x"), py::arg("y"),  py::arg("z"))
            .def_property_readonly("x", &cura::Point3::x)
            .def_property_readonly("y", &cura::Point3::y)
            .def_property_readonly("z", &cura::Point3::z)
            ;

    py::class_<cura::FMatrix4x3>(m, "FMatrix")
            .def(py::init<>())
            .def("apply",
                 [](cura::FMatrix4x3 fm, const cura::FPoint3 & p){
                    return fm.apply(p);
                 })
            ;
}