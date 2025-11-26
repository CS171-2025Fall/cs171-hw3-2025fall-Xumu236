/**
 * @file math_utils.h
 * @author ShanghaiTech CS171 TAs
 * @brief
 * @version 0.1
 * @date 2023-04-13
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef __MATH_UTILS_H__
#define __MATH_UTILS_H__

#include <algorithm>
#include <array>
#include <cmath>
#include <random>

#include "rdr/canary.h"
#include "rdr/math_aliases.h"
#include "rdr/platform.h"

RDR_NAMESPACE_BEGIN

/* ===================================================================== *
 *
 * Math Utils
 *
 * ===================================================================== */

RDR_FORCEINLINE Float CosTheta(const Vec3f &w) {
  return w.z;
}

RDR_FORCEINLINE Float Cos2Theta(const Vec3f &w) {
  return w.z * w.z;
}

RDR_FORCEINLINE Float AbsCosTheta(const Vec3f &w) {
  return std::abs(w.z);
}

RDR_FORCEINLINE Float Sin2Theta(const Vec3f &w) {
  return std::max((Float)0, (Float)1 - Cos2Theta(w));
}

RDR_FORCEINLINE Float SinTheta(const Vec3f &w) {
  return std::sqrt(Sin2Theta(w));
}

RDR_FORCEINLINE Float TanTheta(const Vec3f &w) {
  return SinTheta(w) / CosTheta(w);
}

RDR_FORCEINLINE Float Tan2Theta(const Vec3f &w) {
  return Sin2Theta(w) / Cos2Theta(w);
}

RDR_FORCEINLINE Float CosPhi(const Vec3f &w) {
  Float sin_theta = SinTheta(w);
  return (sin_theta == 0) ? 1 : std::clamp<Float>(w.x / sin_theta, -1, 1);
}

RDR_FORCEINLINE Float SinPhi(const Vec3f &w) {
  Float sin_theta = SinTheta(w);
  return (sin_theta == 0) ? 0 : std::clamp<Float>(w.y / sin_theta, -1, 1);
}

RDR_FORCEINLINE Float Cos2Phi(const Vec3f &w) {
  return CosPhi(w) * CosPhi(w);
}

RDR_FORCEINLINE Float Sin2Phi(const Vec3f &w) {
  return SinPhi(w) * SinPhi(w);
}

RDR_FORCEINLINE Vec3f Reflect(const Vec3f &wo, const Vec3f &n) {
  return -wo + 2 * Dot(wo, n) * n;
}

RDR_FORCEINLINE Vec3f FaceForward(const Vec3f &target, const Vec3f &w) {
  return (Dot(target, w) < 0) ? -w : w;
}

/**
 * @brief Refract a ray with incident direction wi and surface normal n.
 * @param eta The ratio of eta between incident medium and transmitted medium.
 * @return true if the ray is refracted, false if the ray is totally reflected.
 */
RDR_FORCEINLINE bool Refract(
    const Vec3f &wi, const Vec3f &n, Float eta, Vec3f &wt) {
  AssertAllValid(wi, n, eta, wt);
  Float cos_theta_I  = Dot(n, wi);
  Float sin2_theta_I = std::max(0.f, 1.f - cos_theta_I * cos_theta_I);
  Float sin2_theta_T = eta * eta * sin2_theta_I;
  if (sin2_theta_T >= 1) return false;
  Float cos_theta_T = std::sqrt(1 - sin2_theta_T);

  // Write the output
  wt = eta * -wi + (eta * cos_theta_I - cos_theta_T) * Vec3f(n);
  wt = Normalize(wt);
  AssertAllValid(wt);
  AssertAllNormalized(wt);
  return true;
}

RDR_FORCEINLINE bool SameHemisphere(const Vec3f &w, const Vec3f &wp) {
  return w.z * wp.z > 0;
}

RDR_FORCEINLINE Vec3f SphericalDirection(
    Float sin_theta, Float cos_theta, Float phi) {
  return {sin_theta * std::cos(phi), sin_theta * std::sin(phi), cos_theta};
}

RDR_FORCEINLINE Vec3f SphericalDirection(Float theta, Float phi) {
  return SphericalDirection(std::sin(theta), std::cos(theta), phi);
}

RDR_FORCEINLINE Vec3f SphericalDirection(const Vec2f &scoord) {
  return SphericalDirection(scoord[0], scoord[1]);
}

RDR_FORCEINLINE Vec2f InverseSphericalDirection(const Vec3f &w) {
  const auto w_ = Normalize(w);
  Float phi     = std::atan2(w_.y, w_.x);
  if (phi < 0) phi += 2 * PI;
  return {std::acos(w_.z), phi};
}

RDR_FORCEINLINE void CoordinateSystemFromNormal(
    const Vec3f &in_n, Vec3f &x, Vec3f &y) {
  // Don't need to understand.
  // Or you can ask ChatGPT
  using InternalScalarType = Double;
  using InternalVecType    = Vec<InternalScalarType, 3>;

  const InternalVecType n = Cast<InternalScalarType>(in_n);
  if (std::abs(n.x) > std::abs(n.y)) {
    InternalScalarType invLen = 1.0f / std::sqrt(n.x * n.x + n.z * n.z);

    y = Vec3f(n.z * invLen, 0.0f, -n.x * invLen);
  } else {
    InternalScalarType invLen = 1.0f / std::sqrt(n.y * n.y + n.z * n.z);

    y = Vec3f(0.0f, n.z * invLen, -n.y * invLen);
  }

  x = cross(y, in_n);
  AssertAllValid(x, y);
}

constexpr Vec3f DefaultFrameLocalNormal = Vec3f(0, 0, 1);

struct Frame {
  Frame() : n(DefaultFrameLocalNormal) { CoordinateSystemFromNormal(n, x, y); }
  Frame(const Vec3f &n) : n(n) { CoordinateSystemFromNormal(n, x, y); }

  Vec3f WorldToLocal(const Vec3f &v /* world */) const {
    AssertAllValid(v);
    return Normalize(Vec3f(Dot(v, x), Dot(v, y), Dot(v, n)));
  }

  Vec3f LocalToWorld(const Vec3f &v /* local */) const {
    AssertAllValid(v);
    return Normalize(v.x * x + v.y * y + v.z * n);
  }

  Vec3f n, x, y;
};
// Question：请向我解释Frame类的作用和用法。
// Answer：Frame类用于表示一个局部坐标系，通常用于计算机图形学中的光线追踪和渲染。它包含三个正交的基向量n、x和y，分别表示局部坐标系的法线方向和两个切线方向。Frame类提供了两个主要的方法：WorldToLocal和LocalToWorld。WorldToLocal方法将一个在全球坐标系中的向量转换为局部坐标系中的向量，而LocalToWorld方法则将一个在局部坐标系中的向量转换回全球坐标系。这些转换通常通过线性代数操作实现，例如矩阵乘法或向量点积，利用Frame类中存储的基向量来完成坐标变换。使用Frame类可以简化在渲染过程中处理光线和表面交互时的计算，使得代码更加清晰和高效。

/// Definitions for OffsetRayOrigin
namespace detail_ {
RDR_FORCEINLINE float __host_int_as_float(int a) {
  union {
    int m_a;
    float m_b;
  } u;
  u.m_a = a;
  return u.m_b;
}
RDR_FORCEINLINE int __host_float_as_int(float b) {
  union {
    int m_a;
    float m_b;
  } u;
  u.m_b = b;
  return u.m_a;
}
}  // namespace detail_

/**
 * @brief Offset the ray to avoid self-intersection
 * @note This implementation is from
 * https://research.nvidia.com/publication/2019-03_fast-and-robust-method-avoiding-self-intersection,
 * which provides a way to adaptively construct the new position without
 * tweaking the SHADOW_NORMAL param.
 * The observation is that, using a fixed EPS is not scene invariant and scale
 * invariant. We know that floating-point arithmetics's *relative accuracy*
 * remains almost invariant, but its *absolute accuracy* does not. So a scene's
 * EPS with an characteristic size of 10 is absolutely different from a size of
 * 1e9. That is, the absolute accuracy of intersecting a distant triangle is
 * much more lower. So int arithmetics is used, in which the absolute accuracy
 * is maintained.
 *
 * @param p The ray's original position
 * @param n The *geometry* normal
 * @param dir The ray's direction
 * @return Vec3f representing the ray's original position after offset
 */
RDR_FORCEINLINE Vec3f OffsetRayOrigin(const Vec3f &p, const Vec3f &n) {
  // Definition of integer arithmetics functions
  const auto &int_as_float = detail_::__host_int_as_float;
  const auto &float_as_int = detail_::__host_float_as_int;
  if constexpr (std::is_same_v<Float, float>) {
    constexpr float origin      = 1 / 32.0F;
    constexpr float float_scale = 1 / 65536.0F;
    constexpr float int_scale   = 256.0F;
    const Vec3f &offset         = n;
    const Vec3i &offset_int     = Cast<int>(offset * int_scale);
    const Vec3f &p_int{int_as_float(float_as_int(p.x) +
                                    ((p.x < 0) ? -offset_int.x : offset_int.x)),
        int_as_float(
            float_as_int(p.y) + ((p.y < 0) ? -offset_int.y : offset_int.y)),
        int_as_float(
            float_as_int(p.z) + ((p.z < 0) ? -offset_int.z : offset_int.z))};
    return {fabsf(p.x) < origin ? p.x + float_scale * offset.x : p_int.x,
        fabsf(p.y) < origin ? p.y + float_scale * offset.y : p_int.y,
        fabsf(p.z) < origin ? p.z + float_scale * offset.z : p_int.z};
  } else {
    return p + n * EPS;
  }
}

/// [0, 1)
RDR_FORCEINLINE Float Clamp01(Float v) {
  return std::clamp<Float>(v, 0, 1 - Float_EPSILON);
}

template <typename T>
RDR_FORCEINLINE T Mod(T a, T b) {
  if constexpr (std::is_same_v<T, Float>) {
    return std::fmod(a, b);
  } else {
    T result = a - (a / b) * b;
    return (T)((result < 0) ? result + b : result);
  }
}

RDR_FORCEINLINE uint8_t GammaCorrection(float radiance) {
  /// This with result in different result from mitsuba 0.6, whose tone mapper
  /// is really complex. So if you want to precisely debug the renderer, use
  /// exr.
  return static_cast<uint8_t>(255.0F * Clamp01(powf(radiance, 1.F / 2.2F)));
}

RDR_FORCEINLINE Float Radians(Float x) {
  return x * PI / 180;
}

RDR_FORCEINLINE Vec3f DeNan(const Vec3f &vec, float val) {
  Vec3f tmp = vec;
  if (vec.x != vec.x) tmp.x = val;
  if (vec.y != vec.y) tmp.y = val;
  if (vec.z != vec.z) tmp.z = val;
  return tmp;
}

/* ===================================================================== *
 *
 * Sampling-related Utils
 *
 * ===================================================================== */

class Sampler {
public:
  virtual ~Sampler() = default;

  RDR_FORCEINLINE virtual bool resetAfterIteration() { return true; }
  RDR_FORCEINLINE virtual Float get1D() { return Clamp01(dis(engine)); }
  RDR_FORCEINLINE virtual Vec2f get2D() { return {get1D(), get1D()}; }
  RDR_FORCEINLINE virtual void setSeed(int i) { engine.seed(i); }
  RDR_FORCEINLINE virtual void setPixelIndex2D(const Vec2i &index) {
    pixel_index = index;
  }

  RDR_FORCEINLINE virtual const Vec2i &getPixelIndex2D() const {
    return pixel_index;
  }

  RDR_FORCEINLINE virtual Vec2f getPixelSample() {
    return Cast<Float>(pixel_index) + get2D();
  }

  /** Shuffle a given array using the engine */
  template <typename InIterator>
  void shuffle(InIterator begin, InIterator end) {
    std::shuffle(begin, end, engine);
  }

protected:
  Vec2i pixel_index;
  std::mt19937 engine;
  std::uniform_real_distribution<Float> dis{0, 1 - Float_EPSILON};
};

/**
 * @brief Different measure of samples. For example, sampling the triangle
 * produces Area measure. Sampling the hemisphere produces SolidAngle measure.
 * PDF accross different measure should be converted respectively.
 */
enum class EMeasure {
  EUnknownMeasure = 0,
  ESolidAngle,
  EArea,
};

RDR_FORCEINLINE Vec2f UniformSampleDisk(const Vec2f &u) {
  // This is left as the next assignment
  // UNIMPLEMENTED;
  Float r     = sqrt(u.x);
  Float theta = 2 * PI * u.y;
  return r * Vec2f(std::cos(theta), std::sin(theta));
  // Question: 请解释这个函数的实现原理。
  // Answer:
  // 该函数实现了在单位圆盘上均匀采样的功能。首先，通过对输入的二维均匀随机变量u的第一个分量进行平方根变换，得到半径r，这样可以确保采样点在圆盘内均匀分布。然后，通过将第二个分量乘以2π，得到角度theta，从而确定采样点在圆盘上的位置。最后，使用极坐标转换公式，将(r,
  // theta)转换为笛卡尔坐标系下的(x,
  // y)坐标，并返回该坐标作为采样结果。这种方法确保了采样点在圆盘内的均匀分布。
  // Question: 为什么平方根变换会确保采样点在圆盘内均匀分布？
  // Answer:
  // 在二维空间中，均匀分布的点在圆盘内的密度应该与面积成正比。圆盘的面积与半径的平方成正比，因此为了在圆盘内均匀分布采样点，我们需要对半径进行平方根变换。具体来说，如果我们直接使用均匀分布的随机变量作为半径，那么采样点会更集中在圆盘的中心区域，而不是均匀分布在整个圆盘内。通过对随机变量进行平方根变换，我们可以确保采样点在圆盘内的分布是均匀的，从而实现真正的均匀采样。
  // Question：也就是说，这个函数接受两个随机参数的输入，然后用第一个参数确定采样点到圆心的距离，用第二个参数确定采样点的角度，从而在圆盘内均匀分布采样点，是吗？
  // Answer:
  // 是的，您理解得很正确。该函数通过接受两个均匀随机变量作为输入，第一个变量用于确定采样点到圆心的距离（通过平方根变换得到半径r），第二个变量用于确定采样点的角度（通过乘以2π得到角度theta）。这样，函数能够在单位圆盘内均匀分布采样点，确保每个区域的采样概率相等。
  // Question：那么这两个随机变量参数都需要在一定范围内（比如x应该在0到1之间，y应该在0到2pi），对吗？
  // Answer:
  // 实际上，这两个随机变量参数u.x和u.y都应该在[0,
  // 1)的范围内。函数内部会将u.x进行平方根变换以得到半径r，而u.y则通过乘以2π来计算角度theta。因此，输入的随机变量不需要直接在[0,
  // 2π)范围内，而是通过函数内部的转换来实现这一点。这样设计的目的是为了简化采样过程，同时确保采样点在圆盘内均匀分布。
}

RDR_FORCEINLINE Vec3f UniformSampleHemisphere(const Vec2f &u) {
  // This is left as the next assignment
  // UNIMPLEMENTED;
  Float theta = acos(1 - u.x);
  Float phi   = 2 * PI * u.y;
  return SphericalDirection(theta, phi);
}
// Question: 请解释这个函数的实现原理。
// Answer:
// 该函数实现了在单位半球面上均匀采样的功能。首先，通过将输入的二维均匀随机变量u的第一个分量直接作为z坐标，确保采样点在半球面上的高度分布是均匀的。然后，计算半径r，表示采样点在xy平面上的投影距离，使用公式r
// = sqrt(1 -
// z^2)来确保采样点位于单位半球面上。接着，通过将第二个分量乘以2π，得到角度phi，从而确定采样点在xy平面上的位置。最后，使用极坐标转换公式，将(r,
// phi, z)转换为笛卡尔坐标系下的(x, y,
// z)坐标，并返回该坐标作为采样结果。这种方法确保了采样点在单位半球面上的均匀分布。
// Question:
// 为什么直接使用u.x作为z坐标会确保采样点在半球面上的高度分布是均匀的？
// Answer:
// 在单位半球面上，z坐标的范围是从0到1。为了确保采样点在半球面上的高度分布是均匀的，我们可以直接使用均匀分布的随机变量u.x作为z坐标。这样，z坐标在[0,
// 1)范围内均匀分布，从而确保采样点在半球面上的高度分布也是均匀的。通过这种方式，我们避免了复杂的变换，同时保持了采样点在半球面上的均匀分布特性。
// Question：如果我有一个函数，接收theta和phi作为参数，theta是从z轴到采样点的夹角，phi是xy平面上的角度，最后能返回直角坐标。那么我该如何实现UniformSampleHemisphere函数呢？
// Answer:
// 如果您有一个函数可以接受theta和phi作为参数，并返回对应的直角坐标，那么您可以按照以下步骤实现UniformSampleHemisphere函数：
// 1. 首先，计算theta和phi。对于均匀采样的半球面，您可以使用以下公式：
//    - theta = acos(1 - u.x) // 这里u.x是均匀分布的随机变量，范围在[0, 1)
//    - phi = 2 * PI * u.y // 这里u.y也是均匀分布的随机变量，范围在[0, 1)
// 2.
// 然后，调用您已有的函数，将计算得到的theta和phi作为参数传入，获取对应的直角坐标。
// 3. 最后，返回该直角坐标作为采样结果。
// Question：有了这个SphericalDirection函数，看起来sample变得更加简单了，这是为什么？
// Answer:
// SphericalDirection函数简化了采样过程，因为它直接将球面坐标（由theta和phi定义）转换为直角坐标系下的三维向量。这意味着，在实现采样函数时，您不需要手动计算x、y和z坐标，而只需计算theta和phi，然后调用SphericalDirection函数即可获得所需的三维向量。这种封装使得代码更加简洁和易读，同时减少了潜在的错误，因为您不需要重复实现坐标转换的逻辑。总之，SphericalDirection函数提供了一种方便的方式来处理球面坐标与直角坐标之间的转换，从而简化了采样过程。

RDR_FORCEINLINE Vec3f CosineSampleHemisphere(const Vec2f &u) {
  // This is left as the next assignment
  UNIMPLEMENTED;
  return Vec3f();
}

RDR_FORCEINLINE Vec3f UniformSampleSphere(const Vec2f &u) {
  // This is left as the next assignment
  // UNIMPLEMENTED;
  Float theta = acos(1 - 2 * u.x);
  Float phi   = 2 * PI * u.y;
  return SphericalDirection(theta, phi);
}

RDR_FORCEINLINE Vec3f UniformSampleTriangle(const Vec2f &u) {
  Float su0 = std::sqrt(u.x);
  Float b0  = 1 - su0;
  Float b1  = u.y * su0;
  return Vec3f(b0, b1, 1.f - b0 - b1);
}

/*
    pbrt source code is Copyright(c) 1998-2016
                        Matt Pharr, Greg Humphreys, and Wenzel Jakob.

    This file is part of pbrt.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are
    met:

    - Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

    - Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
    IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
    TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
    HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 */
struct Distribution1D {
  // Distribution1D Public Methods
  Distribution1D(const Float *f, int n) : func(f, f + n), cdf(n + 1) {
    // Compute integral of step function at $x_i$
    cdf[0] = 0;
    for (int i = 1; i < n + 1; ++i) cdf[i] = cdf[i - 1] + func[i - 1] / n;

    // Transform step function integral into CDF
    funcInt = cdf[n];
    if (funcInt == 0) {
      for (int i = 1; i < n + 1; ++i) cdf[i] = Float(i) / Float(n);
    } else {
      for (int i = 1; i < n + 1; ++i) cdf[i] /= funcInt;
    }
  }
  int size() const { return (int)func.size(); }
  Float sampleContinuous(Float u, Float *pdf, int *off = nullptr) const {
    // Find surrounding CDF segments and _offset_
    int offset = FindInterval(
        (int)cdf.size(), [&](int index) { return cdf[index] <= u; });
    if (off != nullptr) *off = offset;
    // Compute offset along CDF segment
    Float du = u - cdf[offset];
    if ((cdf[offset + 1] - cdf[offset]) > 0) {
      assert(cdf[offset + 1] > cdf[offset]);
      du /= (cdf[offset + 1] - cdf[offset]);
    }
    IsAllValid(du);

    // Compute PDF for sampled offset
    if (pdf) *pdf = (funcInt > 0) ? func[offset] / funcInt : 0;

    // Return $x\in{}[0,1)$ corresponding to sample
    return (offset + du) / size();
  }
  int sampleDiscrete(
      Float u, Float *pdf = nullptr, Float *uRemapped = nullptr) const {
    // Find surrounding CDF segments and _offset_
    int offset = FindInterval(
        (int)cdf.size(), [&](int index) { return cdf[index] <= u; });
    if (pdf) *pdf = (funcInt > 0) ? func[offset] / (funcInt * size()) : 0;
    if (uRemapped)
      *uRemapped = (u - cdf[offset]) / (cdf[offset + 1] - cdf[offset]);
    if (uRemapped) assert(*uRemapped >= 0.f && *uRemapped <= 1.f);
    return offset;
  }
  Float discretePDF(int index) const {
    assert(index >= 0 && index < size());
    return func[index] / (funcInt * size());
  }
  Float getIntegral() const { return funcInt; }

  // Distribution1D Public Data
  std::vector<Float> func, cdf;
  Float funcInt;

private:
  template <typename Predicate>
  int FindInterval(int size, const Predicate &pred) const {
    int first = 0, len = size;
    while (len > 0) {
      int half = len >> 1, middle = first + half;
      // Bisect range based on value of _pred_ at _middle_
      if (pred(middle)) {
        first = middle + 1;
        len -= half + 1;
      } else
        len = half;
    }
    return std::clamp<int>(first - 1, 0, size - 2);
  }
};

struct BeckmannDistribution {
  Float alpha_x, alpha_y;

  static Float roughnessToAlpha(Float roughness) {
    roughness = std::max(roughness, (Float)1e-3);
    Float x   = std::log(roughness);
    return 1.62142f + 0.819955f * x + 0.1734f * x * x + 0.0171201f * x * x * x +
           0.000640711f * x * x * x * x;
  }

  BeckmannDistribution(Float alpha) : alpha_x(alpha), alpha_y(alpha) {}
  BeckmannDistribution(Float alpha_x, Float alpha_y)
      : alpha_x(alpha_x), alpha_y(alpha_y) {}

  Float D(const Vec3f &wh) const {
    Float tan2_theta = Tan2Theta(wh);
    if (std::isinf(tan2_theta)) return 0.;
    Float cos4_theta = Cos2Theta(wh) * Cos2Theta(wh);
    return std::exp(-tan2_theta * (Cos2Phi(wh) / (alpha_x * alpha_x) +
                                      Sin2Phi(wh) / (alpha_y * alpha_y))) /
           (PI * alpha_x * alpha_y * cos4_theta);
  }

  Float lambda(const Vec3f &w) const {
    Float abs_tan_theta = std::abs(TanTheta(w));
    if (std::isinf(abs_tan_theta)) return 0.;
    Float alpha = std::sqrt(
        Cos2Phi(w) * alpha_x * alpha_x + Sin2Phi(w) * alpha_y * alpha_y);
    Float a = 1 / (alpha * abs_tan_theta);
    if (a >= 1.6f) return 0;
    return (1 - 1.259f * a + 0.396f * a * a) / (3.535f * a + 2.181f * a * a);
  }

  Float G1(const Vec3f &w) const { return 1 / (1 + lambda(w)); }

  Float G(const Vec3f &wo, const Vec3f &wi) const {
    return 1 / (1 + lambda(wo) + lambda(wi));
  }

  Float pdf([[maybe_unused]] const Vec3f &wo, const Vec3f &wh) const {
    return D(wh) * AbsCosTheta(wh);
  }

  Vec3f sampleWh(const Vec3f &wo, const Vec2f &u) const {
    Float tan2_theta;
    Float phi;
    if (alpha_x == alpha_y) {
      Float log_sample = std::log(1 - u[0]);
      tan2_theta       = -alpha_x * alpha_x * log_sample;
      phi              = u[1] * 2 * PI;
    } else {
      // distribution
      Float log_sample = std::log(1 - u[0]);
      phi = std::atan(alpha_y / alpha_x * std::tan(2 * PI * u[1] + 0.5f * PI));
      if (u[1] > 0.5F) phi += PI;
      Float sin_phi    = std::sin(phi);
      Float cos_phi    = std::cos(phi);
      Float m_alpha_x2 = alpha_x * alpha_x;
      Float m_alpha_y2 = alpha_y * alpha_y;
      tan2_theta       = -log_sample / (cos_phi * cos_phi / m_alpha_x2 +
                                     sin_phi * sin_phi / m_alpha_y2);
    }

    Float cos_theta = 1 / std::sqrt(1 + tan2_theta);
    Float sin_theta = std::sqrt(std::max((Float)0, 1 - cos_theta * cos_theta));
    Vec3f wh        = SphericalDirection(sin_theta, cos_theta, phi);
    if (wo.z * wh.z <= 0) wh = -wh;
    return wh;
  }
};

/* ===================================================================== *
 *
 * Photon-mapping related Kernels
 *
 * ===================================================================== */

RDR_FORCEINLINE Float RawEpanechnikovKernel(Float distance, Float radius) {
  Float u = distance / radius;
  return max(1 - u * u, 0);
}

RDR_FORCEINLINE Float EpanechnikovKernel2D(Float distance, Float radius) {
  Float u = distance / radius;
  return max(1 - u * u, 0) * (2 * INV_PI) / (radius * radius);
}

RDR_NAMESPACE_END

#endif
