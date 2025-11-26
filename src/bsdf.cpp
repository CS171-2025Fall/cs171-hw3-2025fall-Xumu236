#include "rdr/bsdf.h"

#include "rdr/fresnel.h"
#include "rdr/interaction.h"
#include "rdr/math_aliases.h"
#include "rdr/platform.h"

RDR_NAMESPACE_BEGIN

static Vec3f obtainOrientedNormal(
    const SurfaceInteraction &interaction, bool twosided) {
  AssertAllValid(interaction.shading.n);
  AssertAllNormalized(interaction.shading.n);
  return twosided && interaction.cosThetaO() < 0 ? -interaction.shading.n
                                                 : interaction.shading.n;
}

/* ===================================================================== *
 *
 * IdealDiffusion
 *
 * ===================================================================== */

void IdealDiffusion::crossConfiguration(
    const CrossConfigurationContext &context) {
  auto texture_name = properties.getProperty<std::string>("texture_name");
  auto texture_ptr  = context.textures.find(texture_name);
  if (texture_ptr != context.textures.end()) {
    texture = texture_ptr->second;
  } else {
    Exception_("Texture [ {} ] not found", texture_name);
  }

  clearProperties();
}

Vec3f IdealDiffusion::evaluate(SurfaceInteraction &interaction) const {
  const Vec3f normal = obtainOrientedNormal(interaction, twosided);
  if (Dot(interaction.wi, normal) < 0 || Dot(interaction.wo, normal) < 0)
    return {0, 0, 0};
  return texture->evaluate(interaction) * INV_PI;
}

Float IdealDiffusion::pdf(SurfaceInteraction &interaction) const {
  // This is left as the next assignment
  // UNIMPLEMENTED;
  return 0;
}

Vec3f IdealDiffusion::sample(
    SurfaceInteraction &interaction, Sampler &sampler, Float *out_pdf) const {
  // This is left as the next assignment
  // UNIMPLEMENTED;
  return Vec3f(0.0);
}

/// return whether the bsdf is perfect transparent or perfect reflection
bool IdealDiffusion::isDelta() const {
  return false;
}

/* ===================================================================== *
 *
 * PerfectRefraction
 *
 * ===================================================================== */

PerfectRefraction::PerfectRefraction(const Properties &props)
    : eta(props.getProperty<Float>("eta", 1.5F)), BSDF(props) {}

Vec3f PerfectRefraction::evaluate(SurfaceInteraction &) const {
  // Since this is a delta distribution, it has no contribution to the queried
  // direction
  return {0.0, 0.0, 0.0};
}

Float PerfectRefraction::pdf(SurfaceInteraction &) const {
  return 0;
}

Vec3f PerfectRefraction::sample(
    SurfaceInteraction &interaction, Sampler &sampler, Float *pdf) const {
  // The interface normal
  Vec3f normal = interaction.shading.n;
  // Cosine of the incident angle
  Float cos_theta_i = Dot(normal, interaction.wo);
  // Whether the ray is entering the medium
  bool entering = cos_theta_i > 0;
  // Corrected eta by direction
  Float eta_corrected = entering ? eta : 1.0F / eta;

  // TODO(HW3): implement the refraction logic here.
  //
  // You should set the `interaction.wi` to the direction of the "in-coming
  // light" after refraction or reflection. Note that `interaction.wi` should
  // always point away from the surface.
  //
  // You may find the following values useful:
  //
  // `interaction.wo`: the out-going view direction, pointing away from the
  // surface.
  // `normal`: the normal of the surface at the interaction point, pointing
  // away from the surface.
  //
  // You may find the following functions useful:
  // @see Refract for refraction calculation.
  // @see Reflect for reflection calculation.

  // UNIMPLEMENTED;
  // My implementation
  // Cast multiple rays per pixel with small offsets for anti-aliasing.
  // For each ray, trace the ray through transparent objects until it first
  // intersects a non-transparent (solid) object. Cast a shadow ray from that
  // intersection point toward the light source to determine visibility. For
  // each visible intersection, compute direct illumination from the light
  // source.
  Vec3f wi;
  bool refracted = Refract(interaction.wo, normal, eta_corrected, wi);
  if (!refracted) {
    print("Total internal reflection occurs in PerfectRefraction BSDF.\n");
    // Total internal reflection occurs, reflect the ray
    wi = Reflect(interaction.wo, normal);
  }
  // print("PerfectRefraction");
  // interaction.wi = wi;
  // 修正：确保wi指向正确的半球
  // 对于折射，wi应该与normal在相反半球
  // Float cos_theta_t = Dot(normal, wi);
  // if (entering && cos_theta_t > 0) {
  //   // 如果应该是进入但wi与normal同向，需要翻转
  //   wi = -wi;
  // } else if (!entering && cos_theta_t < 0) {
  //   // 如果应该是离开但wi与normal反向，需要翻转
  //   wi = -wi;
  // }

  interaction.wi = wi;

  // Set the pdf and return value, we dont need to understand the value now
  if (pdf != nullptr) *pdf = 1.0F;
  return interaction.wi;
}
// Question：上面这个函数还没有用到sampler参数，是不是不太对？
// 答：这里的PerfectRefraction是一个理想的折射BSDF，它的行为是确定性的，不涉及随机采样。因此，在这种情况下，sampler参数实际上并不需要被使用。折射方向是由入射方向和法线通过折射定律唯一确定的，所以不需要额外的随机性来决定方向。
// Question：那这里为什么要设置sampler参数呢？
// 答：sampler参数通常用于那些需要随机采样的BSDF，比如漫反射或光泽反射等。然而，为了保持接口的一致性，所有BSDF的sample函数都包含sampler参数，即使在某些情况下它并不被使用。这种设计使得调用BSDF的代码可以统一处理不同类型的BSDF，而不需要针对每种情况进行特殊处理。

bool PerfectRefraction::isDelta() const {
  return true;
}

/* ===================================================================== *
 *
 * FresnelSpecular
 *
 * ===================================================================== */

Glass::Glass(const Properties &props)
    : R(props.getProperty<Vec3f>("R", Vec3f(1.0))),
      T(props.getProperty<Vec3f>("T", Vec3f(1.0))),
      eta(props.getProperty<Float>("eta", 1.5F)),
      BSDF(props) {}

Vec3f Glass::evaluate(SurfaceInteraction &) const {
  // Since this is a delta distribution, it has no contribution to the queried
  // direction
  return {0.0, 0.0, 0.0};
}

Float Glass::pdf(SurfaceInteraction &) const {
  return 0;
}

Vec3f Glass::sample(
    SurfaceInteraction &interaction, Sampler &sampler, Float *pdf) const {
  // This is left as the next assignment
  // UNIMPLEMENTED;
  return Vec3f(0.0f);
}

bool Glass::isDelta() const {
  return true;
}

/* ===================================================================== *
 *
 * MicrofacetReflection
 *
 * ===================================================================== */

void MicrofacetReflection::crossConfiguration(
    const CrossConfigurationContext &context) {
  auto texture_name = properties.getProperty<std::string>("texture_name");
  auto texture_ptr  = context.textures.find(texture_name);
  if (texture_ptr != context.textures.end()) {
    R = texture_ptr->second;
  } else {
    Exception_("Texture {} not found", texture_name);
  }

  clearProperties();
}

MicrofacetReflection::MicrofacetReflection(const Properties &props)
    : k(props.getProperty<Vec3f>("k", Vec3f(1.0))),
      etaI(props.getProperty<Vec3f>("etaI", Vec3f(1.0F))),
      etaT(props.getProperty<Vec3f>("etaT", Vec3f(1.0F))),
      dist(props.getProperty<Float>("alpha_x", 0.1),
          props.getProperty<Float>("alpha_y", 0.1)),
      BSDF(props) {}

Vec3f MicrofacetReflection::evaluate(SurfaceInteraction &interaction) const {
  // This is left as the next assignment
  // UNIMPLEMENTED;
  return Vec3f(0.0f);
}

Float MicrofacetReflection::pdf(SurfaceInteraction &interaction) const {
  // This is left as the next assignment
  // UNIMPLEMENTED;
  return 0;
}

Vec3f MicrofacetReflection::sample(
    SurfaceInteraction &interaction, Sampler &sampler, Float *pdf_in) const {
  // This is left as the next assignment
  // UNIMPLEMENTED;
  return Vec3f(0.0f);
}

/// return whether the bsdf is perfect transparent or perfect reflection
bool MicrofacetReflection::isDelta() const {
  return false;
}

RDR_NAMESPACE_END
