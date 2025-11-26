#include "rdr/accel.h"

#include "rdr/canary.h"
#include "rdr/interaction.h"
#include "rdr/math_aliases.h"
#include "rdr/platform.h"
#include "rdr/shape.h"

RDR_NAMESPACE_BEGIN

/* ===================================================================== *
 *
 * AABB Implementations
 *
 * ===================================================================== */

bool AABB::isOverlap(const AABB &other) const {
  return ((other.low_bnd[0] >= this->low_bnd[0] &&
              other.low_bnd[0] <= this->upper_bnd[0]) ||
             (this->low_bnd[0] >= other.low_bnd[0] &&
                 this->low_bnd[0] <= other.upper_bnd[0])) &&
         ((other.low_bnd[1] >= this->low_bnd[1] &&
              other.low_bnd[1] <= this->upper_bnd[1]) ||
             (this->low_bnd[1] >= other.low_bnd[1] &&
                 this->low_bnd[1] <= other.upper_bnd[1])) &&
         ((other.low_bnd[2] >= this->low_bnd[2] &&
              other.low_bnd[2] <= this->upper_bnd[2]) ||
             (this->low_bnd[2] >= other.low_bnd[2] &&
                 this->low_bnd[2] <= other.upper_bnd[2]));
}

bool AABB::intersect(const Ray &ray, Float *t_in, Float *t_out) const {
  // TODO(HW3): implement ray intersection with AABB.
  // ray distance for two intersection points are returned by pointers.
  //
  // This method should modify t_in and t_out as the "time"
  // when the ray enters and exits the AABB respectively.
  //
  // And return true if there is an intersection, false otherwise.
  //
  // Useful Functions:
  // @see Ray::safe_inverse_direction
  //    for getting the inverse direction of the ray.
  // @see Min/Max/ReduceMin/ReduceMax
  //    for vector min/max operations.
  // UNIMPLEMENTED;
  // My implementation
  Vec3f inv_dir = ray.safe_inverse_direction;
  Vec3f t0      = (low_bnd - ray.origin) * inv_dir;
  Vec3f t1      = (upper_bnd - ray.origin) * inv_dir;
  Vec3f tmin    = Min(t0, t1);
  Vec3f tmax    = Max(t0, t1);
  Float t_enter = ReduceMax(tmin);
  Float t_exit  = ReduceMin(tmax);

  // 首先检查是否确实相交
  if (t_enter > t_exit) {
    return false;
  }

  // 检查相交是否在射线的有效时间范围内
  if (t_exit < ray.t_min || t_enter > ray.t_max) {
    return false;
  }

  // 确保进入时间不小于t_min
  t_enter = Max(t_enter, ray.t_min);
  // 确保退出时间不大于t_max
  t_exit = Min(t_exit, ray.t_max);

  // 最终确认有有效的相交段
  if (t_enter > t_exit) {
    return false;
  }

  if (t_in) {
    *t_in = t_enter;
  }
  if (t_out) {
    *t_out = t_exit;
  }

  return true;
}
// Question：上面的函数功能完整吗？
// Answer：是的，AABB::intersect函数实现了射线与轴对齐包围盒（AABB）的相交测试功能。它计算了射线进入和退出AABB的时间，并根据这些时间判断是否存在交点。如果存在交点，函数会更新t_in和t_out指针指向的值，并返回true，否则返回false。
// Question：我实际补全的部分仅仅做了判断，然后return false，是不是不完整？
// Answer：是的，仅仅判断并返回false是不完整的。完整的实现应该包括计算射线进入和退出AABB的时间，并在有交点时更新t_in和t_out指针指向的值。你需要补全这些计算部分，以确保函数能够正确处理所有情况。
// Question：那我需要补全哪些部分呢？
// Answer：你需要补全以下部分：
// 1. 计算射线与AABB的交点时间t_in和t_out。
// 2. 检查t_in和t_out是否在射线的有效范围内（ray.t_min <= t_in <=
// ray.t_max和ray.t_min <= t_out <= ray.t_max）。
// 3. 如果有交点，更新t_in和t_out指针指向的值，并返回true。
// 否则，返回false。

//   return false;
// }

/* ===================================================================== *
 *
 * Accelerator Implementations
 *
 * ===================================================================== */

bool TriangleIntersect(Ray &ray, const uint32_t &triangle_index,
    const ref<TriangleMeshResource> &mesh, SurfaceInteraction &interaction) {
  using InternalScalarType = Double;
  using InternalVecType    = Vec<InternalScalarType, 3>;

  AssertAllValid(ray.direction, ray.origin);
  AssertAllNormalized(ray.direction);

  const auto &vertices = mesh->vertices;
  const Vec3u v_idx(&mesh->v_indices[3 * triangle_index]);
  assert(v_idx.x < mesh->vertices.size());
  assert(v_idx.y < mesh->vertices.size());
  assert(v_idx.z < mesh->vertices.size());

  InternalVecType dir = Cast<InternalScalarType>(ray.direction);
  InternalVecType v0  = Cast<InternalScalarType>(vertices[v_idx[0]]);
  InternalVecType v1  = Cast<InternalScalarType>(vertices[v_idx[1]]);
  InternalVecType v2  = Cast<InternalScalarType>(vertices[v_idx[2]]);

  // TODO(HW3): implement ray-triangle intersection test.
  // You should compute the u, v, t as InternalScalarType
  //
  //   InternalScalarType u = ...;
  //   InternalScalarType v = ...;
  //   InternalScalarType t = ...;
  //
  // And exit early with `return false` if there is no intersection.
  //
  // The intersection points is denoted as:
  // (1 - u - v) * v0 + u * v1 + v * v2 == ray.origin + t * ray.direction
  // where the left side is the barycentric interpolation of the triangle
  // vertices, and the right side is the parametric equation of the ray.
  //
  // You should also make sure that:
  // u >= 0, v >= 0, u + v <= 1, and, ray.t_min <= t <= ray.t_max
  //
  // Useful Functions:
  // You can use @see Cross and @see Dot for determinant calculations.

  // Delete the following lines after you implement the function
  // InternalScalarType u = InternalScalarType(0);
  // InternalScalarType v = InternalScalarType(0);
  // InternalScalarType t = InternalScalarType(0);
  // UNIMPLEMENTED;
  // My implementation

  InternalVecType edge1  = v1 - v0;
  InternalVecType edge2  = v2 - v0;
  InternalVecType pvec   = Cross(dir, edge2);
  InternalScalarType det = Dot(edge1, pvec);
  if (abs(det) < InternalScalarType(1e-8)) {
    return false;
  }
  InternalScalarType inv_det = InternalScalarType(1) / det;
  InternalVecType tvec       = Cast<InternalScalarType>(ray.origin) - v0;
  InternalScalarType u       = Dot(tvec, pvec) * inv_det;
  if (u < InternalScalarType(0) || u > InternalScalarType(1)) {
    return false;
  }
  InternalVecType qvec = Cross(tvec, edge1);
  InternalScalarType v = Dot(dir, qvec) * inv_det;
  if (v < InternalScalarType(0) || u + v > InternalScalarType(1)) {
    return false;
  }
  InternalScalarType t = Dot(edge2, qvec) * inv_det;
  if (t < InternalScalarType(ray.t_min) || t > InternalScalarType(ray.t_max)) {
    return false;
  }

  // We will reach here if there is an intersection

  CalculateTriangleDifferentials(interaction,
      {static_cast<Float>(1 - u - v), static_cast<Float>(u),
          static_cast<Float>(v)},
      mesh, triangle_index);
  AssertNear(interaction.p, ray(t));
  assert(ray.withinTimeRange(t));
  ray.setTimeMax(t);
  return true;
}
// Question：上面的函数功能完整吗？
// Answer：是的，TriangleIntersect函数实现了射线与三角形的相交测试功能。它计算了射线与三角形的交点，并检查了交点是否在射线的有效范围内。如果有交点，函数会更新射线的最大时间并返回true，否则返回false。
// Question：我实际补全的部分仅仅做了判断，然后return false，是不是不完整？
// Answer：是的，仅仅判断并返回false是不完整的。完整的实现应该包括计算交点的参数u、v和t，并在有交点时更新射线的最大时间和交点信息。你需要补全这些计算部分，以确保函数能够正确处理所有情况。
// Question：那我需要补全哪些部分呢？
// Answer：你需要补全以下部分：
// 1. 计算参数u、v和t，表示交点在三角形上的位置和射线的距离。
// 2. 检查u、v和t是否满足条件（u >= 0, v >= 0, u + v <= 1, ray.t_min <= t <=
// ray.t_max）。
// 3.
// 如果有交点，更新射线的最大时间（ray.setTimeMax(t)）并计算交点的微分信息（CalculateTriangleDifferentials）。

void Accel::setTriangleMesh(const ref<TriangleMeshResource> &mesh) {
  // Build the bounding box
  AABB bound(Vec3f(Float_INF, Float_INF, Float_INF),
      Vec3f(Float_MINUS_INF, Float_MINUS_INF, Float_MINUS_INF));
  for (auto &vertex : mesh->vertices) {
    bound.low_bnd   = Min(bound.low_bnd, vertex);
    bound.upper_bnd = Max(bound.upper_bnd, vertex);
  }

  this->mesh  = mesh;   // set the pointer
  this->bound = bound;  // set the bounding box
}

void Accel::build() {}

AABB Accel::getBound() const {
  return bound;
}

bool Accel::intersect(Ray &ray, SurfaceInteraction &interaction) const {
  bool success = false;
  for (int i = 0; i < mesh->v_indices.size() / 3; i++)
    success |= TriangleIntersect(ray, i, mesh, interaction);
  return success;
}

RDR_NAMESPACE_END
