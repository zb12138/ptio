#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include "PCCPointSet.h"
#include "ply.h"
using namespace pcc;
using namespace pcc::ply;
namespace py = pybind11;

class pcc_io {
public:
    PCCPointSet3 cloud;

    void read(const std::string &filename,double positionScale = 1.0) {
        PropertyNameMap attributeNames;
        attributeNames.position = {"x", "y", "z"};
        bool success = ply::read(filename, attributeNames, positionScale, cloud);
        if (!success) throw std::runtime_error("Failed to read PLY file.");
    }

    void write(const std::string &filename, double positionScale = 1.0, bool asAscii = false) {
        PropertyNameMap attributeNames;
        attributeNames.position = {"x", "y", "z"};
        bool success = ply::write(cloud, attributeNames, positionScale,{0,0,0} , filename, asAscii);
        if (!success) throw std::runtime_error("Failed to write PLY file.");
    }

    py::array_t<double> get_points() {
        size_t pointCount = cloud.getPointCount();
        // std::cout << "Point count: " << pointCount << std::endl;
        auto result = py::array_t<double>(pointCount * 3);
        auto r = result.mutable_unchecked<1>();

        for (size_t i = 0; i < pointCount; ++i) {
            const auto &point = cloud[i];
            r(i * 3) = point[0];
            r(i * 3 + 1) = point[1];
            r(i * 3 + 2) = point[2];
        }
        return result;
    }

    py::array_t<attr_t> get_colors() {
        size_t pointCount = cloud.getPointCount();
        auto result = py::array_t<attr_t>(pointCount * 3);
        auto r = result.mutable_unchecked<1>();
        if (cloud.hasColors()) {
            for (size_t i = 0; i < pointCount; ++i) {
                const Vec3<attr_t>& c = cloud.getColor(i);//GBR
                r(i * 3 ) = c[2];
                r(i * 3 + 1) = c[0];
                r(i * 3 + 2) = c[1];
            }
        }
        return result;
    }
    py::array_t<attr_t> get_reflectance() {
        size_t pointCount = cloud.getPointCount();
        auto result = py::array_t<attr_t>(pointCount);
        auto r = result.mutable_unchecked<1>();
        if (cloud.hasReflectances()) {
            for (size_t i = 0; i < pointCount; ++i) {
                r(i) = cloud.getReflectance(i);
            }
        }
        return result;
    }

    void set_points(const py::array_t<double> &points) {
        auto p = points.unchecked<1>();
        size_t pointCount = p.size() / 3;
        cloud.resize(pointCount);
        for (size_t i = 0; i < pointCount; ++i) {
            cloud[i] = Vec3<double>(p[i * 3], p[i * 3 + 1], p[i * 3 + 2]);
        }
    }

    void set_colors(const py::array_t<attr_t> &colors) {
        auto c = colors.unchecked<1>();
        size_t pointCount = c.size() / 3;
        cloud.addColors();
        for (size_t i = 0; i < pointCount; ++i) {
            cloud.getColor(i) = Vec3<attr_t>(c[i * 3 + 1], c[i * 3 + 2], c[i * 3]); // GBR
        }
    }

    void set_reflectance(const py::array_t<attr_t> &reflectance) {
        auto r = reflectance.unchecked<1>();
        size_t pointCount = r.size();
        cloud.addReflectances();
        for (size_t i = 0; i < pointCount; ++i) {
            cloud.getReflectance(i) = r(i);
        }
    }

    void clear() {
        cloud.clear();
    }

    void remove_colors() {
        cloud.removeColors();
    }

    void remove_reflectance() {
        cloud.removeReflectances();
    }

    //get point count
    size_t get_point_count() const {
        return cloud.getPointCount();
    }

    //has color
    bool has_colors() const {
        return cloud.hasColors();
    }
    //has reflectance
    bool has_reflectance() const {
        return cloud.hasReflectances();
    }
};

PYBIND11_MODULE(ptio_backend, m) {
    py::class_<pcc_io>(m, "PCC_IO")
        .def(py::init<>())
        .def("read", &pcc_io::read)
        .def("write", &pcc_io::write)
        .def("clear", &pcc_io::clear)
        .def("get_points", &pcc_io::get_points)
        .def("get_colors", &pcc_io::get_colors)
        .def("get_reflectance", &pcc_io::get_reflectance)
        .def("set_points", &pcc_io::set_points)
        .def("set_colors", &pcc_io::set_colors)
        .def("set_reflectance", &pcc_io::set_reflectance)
        .def("get_point_count", &pcc_io::get_point_count)
        .def("has_colors", &pcc_io::has_colors)
        .def("has_reflectance", &pcc_io::has_reflectance)
        .def("remove_colors", &pcc_io::remove_colors)
        .def("remove_reflectance", &pcc_io::remove_reflectance);
}