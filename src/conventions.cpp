/**
 * @file conventions.cpp
 * @author ShanghaiTech CS171 TAs
 * @brief This file intends to familar you with those basic components in this
 * renderer, as well as those math conventions. Everyone SHOULD carefully go
 * through this file.
 * @version 0.1
 * @date 2023-04-12
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <numeric>

#include "rdr/rdr.h"

using namespace RDR_NAMESPACE_NAME;

int main() {
  // Handedness Convention
  Vec3f x(1, 0, 0);
  Vec3f y(0, 1, 0);
  auto z = Cross(x, y);  // no matter

  std::string text1 = R"(
// The renderer is a right-handed system
// You can imagine the scene as x pointing to the right, y pointing up, and z
// pointing towards you, which respect the right-hand rule. Note that this is
// for global coordinate, i.e. the scene setting.
)";
  print("{}// [1] z = Cross(x, y) = {}\n", text1, z);

  auto text2 = R"(
// Meanwhile the local coordinate system of hand is right. Where *z* is the
// principal axis pointing up. WHICH IS DIFFERENT FROM THE GLOBAL COORDINATE
// For example,
)";
  Vec3f direction(1, 0, 1);
  direction = Normalize(direction);
  print("{}// [2] direction = {}\n", text2, direction);
  print("// [3] cosTheta = Dot(direction, y) = {}\n", Dot(direction, z));
  print("// [4] theta = acos(cosTheta) = {} = pi/4\n", acos(Dot(direction, z)));

  auto text3 = R"(
// For sphericala coordinates, we uses theta for vertical index, and phi for
// horizontal index. That is to say, 0 <= theta < pi and 0 <= phi < 2pi.
)";
  print("{}// [5] SphericalDirection(theta=0, phi=0)   = {}\n", text3,
      SphericalDirection(0, 0));
  print("// [6] SphericalDirection(theta=pi/2, 0)    = {}\n",
      SphericalDirection(PI / 2, 0));
  auto vec_ = SphericalDirection(PI / 2, PI / 4);
  print("// [7] SphericalDirection(theta=pi/2, pi/4) = {}\n", vec_);
  print("// [8] InverseSphericalDirection({}) = {}\n", vec_,
      InverseSphericalDirection(vec_));

  Vec3f n(0, 0, 1), x_, y_;
  CoordinateSystemFromNormal(n, x_, y_);
  auto text4 = R"(
// Let's examine the coordinate transition system, which will be heavily used
// in all components. We build a coordinate system:
)";
  print("{}// [9] x_ = {}, y_ = {}, Cross(x_, y_) = {}\n", text4, x_, y_,
      Cross(x_, y_));

  // We wrap this mechanism with frame.
  Frame frame(Vec3f(0, 1, 0));
  Vec3f gvec(0, 1, 0);
  auto lvec = frame.WorldToLocal(gvec);
  auto rvec = frame.LocalToWorld(lvec);
  print("// [10] original_vec = {}, local_vec = {}\n", gvec, lvec);

  Vec3f a(1, 1, 2);
  print("// [11] a = {}, a*2 = {}, size = {}\n", a, a * 2, sizeof(a));
}

// Questions：我知道这个脚本是用来说明坐标系和一些数学约定的，请你帮我用中文概括一下这些约定。
// 回答：这个脚本说明了渲染器使用的是右手坐标系，全球坐标系中x轴指向右方，y轴指向上方，z轴指向观察者方向。同时，局部坐标系中z轴作为主轴指向上方。脚本还介绍了球面坐标系的定义，其中theta表示垂直角度，phi表示水平角度。此外，脚本展示了如何从法线构建坐标系，并使用Frame类进行世界坐标和局部坐标之间的转换。这些约定对于理解渲染器中的几何计算和光线追踪非常重要。
// Question：我还不太懂球面坐标系的相关约定。运行代码后，我发现输出中提示SphericalDirection(theta=pi/2,
// 0)    = {1,0,-4.371e-08}，请你帮我解释一下这个输出结果代表什么意思。
// Answer：在球面坐标系中，点的位置是通过两个角度来表示的：theta（垂直角度）和phi（水平角度）。当theta=pi/2时，表示点位于赤道平面上，即与z轴垂直的位置。phi=0表示点在x轴的正方向上。因此，SphericalDirection(theta=pi/2,
// 0)计算出的结果{1, 0, -4.371e-08}表示一个接近于(1, 0,
// 0)的向量，这个向量指向x轴的正方向，并且由于数值计算的精度限制，z分量非常接近于零（-4.371e-08）。这个结果说明在球面坐标系中，当theta为90度（pi/2弧度）且phi为0时，点确实位于x轴正方向上。
// Question：也就是说，这里使用的是local坐标系，而不是global坐标系，对吗？
// Answer：是的，SphericalDirection函数返回的向量是基于局部坐标系的。在局部坐标系中，z轴通常被定义为主轴，指向上方，而x轴和y轴则定义了水平平面。因此，当你使用SphericalDirection函数时，得到的结果是相对于这个局部坐标系的方向向量，而不是全球坐标系的方向向量。这种局部坐标系的使用在渲染和光线追踪中非常常见，因为它简化了许多计算和变换过程。
// Question：明白了，谢谢你！还有一个问题，代码中提到的Frame类是什么？它是如何实现local和global坐标系转换的？
// Answer：Frame类是一个用于表示局部坐标系的类，通常包含三个正交的基向量（通常称为tangent、bitangent和normal），这些向量定义了局部坐标系相对于全球坐标系的方向。Frame类提供了两个主要的方法：WorldToLocal和LocalToWorld。WorldToLocal方法将一个在全球坐标系中的向量转换为局部坐标系中的向量，而LocalToWorld方法则将一个在局部坐标系中的向量转换回全球坐标系。这些转换通常通过线性代数操作实现，例如矩阵乘法或向量点积，利用Frame类中存储的基向量来完成坐标变换。这种机制使得在渲染过程中处理光线和表面交互时更加方便和高效。
