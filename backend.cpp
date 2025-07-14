// ptio_backend.cpp
#include "PCCPointSet.h"
#include "ply.h"
#include "backend.h"
#include <Python.h>

using namespace pcc;
using namespace pcc::ply;

// C++实现
pcc_io* pcc_io_create() {
    pcc_io* obj = new pcc_io;
    obj->cloud = new PCCPointSet3;
    return obj;
}

void pcc_io_destroy(pcc_io* obj) {
    if (obj) {
        delete static_cast<PCCPointSet3*>(obj->cloud);
        delete obj;
    }
}

bool pcc_io_read(pcc_io* obj, const char* filename, double positionScale) {
    if (!obj) return false;
    PCCPointSet3* cloud = static_cast<PCCPointSet3*>(obj->cloud);
    PropertyNameMap attributeNames;
    attributeNames.position = {"x", "y", "z"};
    return ply::read(filename, attributeNames, positionScale, *cloud);
}

bool pcc_io_write(pcc_io* obj, const char* filename, double positionScale, bool asAscii) {
    if (!obj) return false;
    PCCPointSet3* cloud = static_cast<PCCPointSet3*>(obj->cloud);
    PropertyNameMap attributeNames;
    attributeNames.position = {"x", "y", "z"};
    return ply::write(*cloud, attributeNames, positionScale, {0,0,0}, filename, asAscii);
}

size_t pcc_io_get_point_count(const pcc_io* obj) {
    if (!obj) return 0;
    return static_cast<const PCCPointSet3*>(obj->cloud)->getPointCount();
}

bool pcc_io_has_colors(const pcc_io* obj) {
    if (!obj) return false;
    return static_cast<const PCCPointSet3*>(obj->cloud)->hasColors();
}

bool pcc_io_has_reflectance(const pcc_io* obj) {
    if (!obj) return false;
    return static_cast<const PCCPointSet3*>(obj->cloud)->hasReflectances();
}

bool pcc_io_get_points(const pcc_io* obj, double** points, size_t* pointCount) {
    if (!obj || !points || !pointCount) return false;
    
    const PCCPointSet3* cloud = static_cast<const PCCPointSet3*>(obj->cloud);
    *pointCount = cloud->getPointCount();
    if (*pointCount == 0) return true;
    
    size_t dataSize = (*pointCount) * 3;
    *points = new double[dataSize];
    
    for (size_t i = 0; i < *pointCount; ++i) {
        const auto& point = (*cloud)[i];
        (*points)[i * 3] = point[0];
        (*points)[i * 3 + 1] = point[1];
        (*points)[i * 3 + 2] = point[2];
    }
    
    return true;
}

bool pcc_io_get_colors(const pcc_io* obj, uint8_t** colors, size_t* pointCount) {
    if (!obj || !colors || !pointCount) return false;
    
    const PCCPointSet3* cloud = static_cast<const PCCPointSet3*>(obj->cloud);
    *pointCount = cloud->getPointCount();
    if (*pointCount == 0 || !cloud->hasColors()) return true;
    
    size_t dataSize = (*pointCount) * 3;
    *colors = new uint8_t[dataSize];
    
    for (size_t i = 0; i < *pointCount; ++i) {
        const Vec3<attr_t>& c = cloud->getColor(i); // GBR
        (*colors)[i * 3] = c[2];     // R
        (*colors)[i * 3 + 1] = c[0]; // G
        (*colors)[i * 3 + 2] = c[1]; // B
    }
    
    return true;
}

bool pcc_io_get_reflectance(const pcc_io* obj, uint16_t** reflectance, size_t* pointCount) {
    if (!obj || !reflectance || !pointCount) return false;
    
    const PCCPointSet3* cloud = static_cast<const PCCPointSet3*>(obj->cloud);
    *pointCount = cloud->getPointCount();
    if (*pointCount == 0 || !cloud->hasReflectances()) return true;
    
    *reflectance = new uint16_t[*pointCount];
    
    for (size_t i = 0; i < *pointCount; ++i) {
        (*reflectance)[i] = cloud->getReflectance(i);
    }
    
    return true;
}

bool pcc_io_set_points(pcc_io* obj, const double* points, size_t pointCount) {
    if (!obj || !points || pointCount == 0) return false;
    
    PCCPointSet3* cloud = static_cast<PCCPointSet3*>(obj->cloud);
    cloud->resize(pointCount);
    
    for (size_t i = 0; i < pointCount; ++i) {
        (*cloud)[i] = Vec3<double>(points[i * 3], points[i * 3 + 1], points[i * 3 + 2]);
    }
    
    return true;
}

bool pcc_io_set_colors(pcc_io* obj, const uint8_t* colors, size_t pointCount) {
    if (!obj || !colors || pointCount == 0) return false;
    
    PCCPointSet3* cloud = static_cast<PCCPointSet3*>(obj->cloud);
    cloud->addColors();
    
    for (size_t i = 0; i < pointCount; ++i) {
        cloud->getColor(i) = Vec3<attr_t>(colors[i * 3 + 1], colors[i * 3 + 2], colors[i * 3]); // GBR
    }
    
    return true;
}

bool pcc_io_set_reflectance(pcc_io* obj, const uint16_t* reflectance, size_t pointCount) {
    if (!obj || !reflectance || pointCount == 0) return false;
    
    PCCPointSet3* cloud = static_cast<PCCPointSet3*>(obj->cloud);
    cloud->addReflectances();
    
    for (size_t i = 0; i < pointCount; ++i) {
        cloud->getReflectance(i) = reflectance[i];
    }
    
    return true;
}

void pcc_io_clear(pcc_io* obj) {
    if (!obj) return;
    static_cast<PCCPointSet3*>(obj->cloud)->clear();
}

void pcc_io_remove_colors(pcc_io* obj) {
    if (!obj) return;
    static_cast<PCCPointSet3*>(obj->cloud)->removeColors();
}

void pcc_io_remove_reflectance(pcc_io* obj) {
    if (!obj) return;
    static_cast<PCCPointSet3*>(obj->cloud)->removeReflectances();
}

void pcc_io_free_double_array(double* arr) {
    delete[] arr;
}

void pcc_io_free_uint8_array(uint8_t* arr) {
    delete[] arr;
}

void pcc_io_free_uint16_array(uint16_t* arr) {
    delete[] arr;
}



static PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "ptio_backend",
    NULL,
    -1,
    NULL, NULL, NULL, NULL, NULL
};

PyMODINIT_FUNC PyInit_ptio_backend(void) {
    return PyModule_Create(&moduledef);
}