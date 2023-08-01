#ifndef JMLANG_BASE_BUFFER_T_H
#define JMLANG_BASE_BUFFER_T_H

#include <cstdint>

/// Defines the internal runtime representation of a buffer_t.
typedef struct buffer_t {
  /// A device handle for e.g. GPU memory.
  uint64_t dev;

  /// A pointer of data in main memory.
  uint8_t* host;

  /// The size of the buffer in each dimension.
  int32_t extent[4];

  /// Gives the spacing in memory between adjacent elements in the
  /// given dimension.  The correct memory address for a load from
  /// this buffer at position x, y, z, w is:
  /// host + (x * stride[0] + y * stride[1] + z * stride[2] + w * stride[3]) *
  /// elem_size By manipulating the strides and extents you can lazily crop,
  /// transpose, and even flip buffers without modifying the data.
  int32_t stride[4];

  /// Buffers often represent evaluation of a Func over some
  /// domain. The min field encodes the top left corner of the domain.
  int32_t min[4];

  /// How many bytes does each buffer element take. This may be
  /// replaced with a more general type code in the future.
  int32_t elem_size;

  /// This should be true if there is an existing device allocation
  /// mirroring this buffer, and the data has been modified on the
  /// host side.
  bool host_dirty;

  /// This should be true if there is an existing device allocation
  /// mirroring this buffer, and the data has been modified on the
  /// device side.
  bool dev_dirty;
} buffer_t;

#endif  // JMLANG_BASE_BUFFER_T_H