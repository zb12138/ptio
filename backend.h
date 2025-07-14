// ptio_backend_capi.h
#ifndef PTIO_BACKEND_CAPI_H
#define PTIO_BACKEND_CAPI_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void* cloud; // 实际是PCCPointSet3*，但C语言中不能直接使用C++类
} pcc_io;

// 创建和销毁对象
pcc_io* pcc_io_create();
void pcc_io_destroy(pcc_io* obj);

// 文件操作
bool pcc_io_read(pcc_io* obj, const char* filename, double positionScale);
bool pcc_io_write(pcc_io* obj, const char* filename, double positionScale, bool asAscii);

// 点云数据获取
size_t pcc_io_get_point_count(const pcc_io* obj);
bool pcc_io_has_colors(const pcc_io* obj);
bool pcc_io_has_reflectance(const pcc_io* obj);

// 数据获取和设置函数
bool pcc_io_get_points(const pcc_io* obj, double** points, size_t* pointCount);
bool pcc_io_get_colors(const pcc_io* obj, uint8_t** colors, size_t* pointCount);
bool pcc_io_get_reflectance(const pcc_io* obj, uint16_t** reflectance, size_t* pointCount);

bool pcc_io_set_points(pcc_io* obj, const double* points, size_t pointCount);
bool pcc_io_set_colors(pcc_io* obj, const uint8_t* colors, size_t pointCount);
bool pcc_io_set_reflectance(pcc_io* obj, const uint16_t* reflectance, size_t pointCount);

// 清除和移除属性
void pcc_io_clear(pcc_io* obj);
void pcc_io_remove_colors(pcc_io* obj);
void pcc_io_remove_reflectance(pcc_io* obj);


void pcc_io_free_double_array(double* arr);
void pcc_io_free_uint8_array(uint8_t* arr);
void pcc_io_free_uint16_array(uint16_t* arr);
#ifdef __cplusplus
}
#endif

#endif // PTIO_BACKEND_CAPI_H