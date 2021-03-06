#include "photon_simulator.h"

#include "environment/environment.h"
#include "environment/meteorology.h"

// extend third party library
namespace _462 {
/**
 * Returns the Hadamard product of two vectors
 */
inline Vector3 Hadamard(const Vector3 &lhs, const Vector3 &rhs) {
  return Vector3(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
}
}  // namespace _462

namespace simulator {

namespace photonsimulator {

PhotonSimulator::PhotonSimulator(const int number, const _462::real_t distance,
                                 const _462::real_t height)
    : num_of_photons_near_by_(number),
      max_distance_(distance),
      sun_height_(height),
      kdtree_(nullptr) {}

void PhotonSimulator::SimulateToTime(
    environment::Environment *env,
    const std::chrono::system_clock::time_point &time) {
  // the part for model
  FreeModels();
  LoadModels(env);

  // the part for photon
  /**
   * TODO: check terrain orientation
   * we regard north as y asix
   */
  // TODO: Since the sun info may not be correct, we may need to fix this.
  _462::Vector3 sun_dir(-sin(env->meteorology().solar_azimuth()),
                        -cos(env->meteorology().solar_azimuth()),
                        -cos(env->meteorology().solar_inclination()));
  _462::Vector3 sun_strength(env->meteorology().hourly_total_irradiance(),
                             env->meteorology().hourly_total_irradiance(),
                             env->meteorology().hourly_total_irradiance());
  double latitude_diff = (env->config().location.latitude_top -
                          env->config().location.latitude_bottom) /
                         100.0f;
  double longtitude_diff = (env->config().location.longitude_right -
                            env->config().location.longitude_left) /
                           100.0f;
  alive_photons_.clear();
  absorb_photons_.clear();
  PhotonEmit(sun_dir, sun_strength, env->config().location.latitude_bottom,
             env->config().location.latitude_top, latitude_diff,
             env->config().location.longitude_left,
             env->config().location.longitude_right, longtitude_diff);

  PhotonsModify();

  // write result to env
  WriteResultToEnv(env);
  // TODO: add functionality if render is required.
}

void PhotonSimulator::FreeModels() {
  models_.clear();
}

// TODO: implement these two functions after refining class plant
void PhotonSimulator::LoadModels(environment::Environment *env) {}
void PhotonSimulator::WriteResultToEnv(environment::Environment *env) {}

void PhotonSimulator::PhotonEmit(
    const _462::Vector3 &sun_direction, const _462::Vector3 &sun_strength,
    const double latitude_bottom, const double latitude_top,
    const double latitudeDiff, const double longitude_left,
    const double longitude_right, const double longitudeDiff) {
  for (_462::real_t i = (_462::real_t)latitude_bottom;
       i <= (_462::real_t)latitude_top; i += (_462::real_t)latitudeDiff) {
    for (_462::real_t j = (_462::real_t)longitude_left;
         j <= (_462::real_t)longitude_right; j += (_462::real_t)longitudeDiff) {
      alive_photons_.push_back(Photon(
          sun_direction, _462::Vector3(i, j, sun_height_), sun_strength));
    }
  }
}

void PhotonSimulator::ConstructKDTree(std::vector<Photon> &p) {
  pointVec points;
  for (const auto photon : p) {
    point_t pt = {photon.pos.x, photon.pos.y, photon.pos.z};
    points.push_back(pt);
  }
  kdtree_ = kdtree_ = std::make_shared<KDTree>(points);
}

indexArr PhotonSimulator::LookuptKDTree(const _462::Vector3 &point,
                                        const _462::real_t distance) const {
  point_t pt = {point.x, point.y, point.z};
  return kdtree_->neighborhood_indices(pt, distance);
}

_462::Vector3 PhotonSimulator::GetPixelColor(const _462::Vector3 &ray_pos,
                                             const _462::Vector3 &ray_dir) {
  _462::Vector3 direct, global;
  const Face *min_face = nullptr;
  const Mesh *min_mesh = nullptr;
  const Model *min_model = nullptr;
  std::tie(min_model, min_mesh, min_face) =
      FindFirstIntersect(ray_pos, ray_dir);
  _462::Vector3 result;
  if (min_face != nullptr) {
    _462::Vector3 intersect =
        min_model->GetIntersect(*min_face, ray_pos, ray_dir);
    direct = min_model->GetFaceTextureColor(*min_face, *min_mesh, intersect);

    global = _462::Vector3(0.0, 0.0, 0.0);
    auto res = LookuptKDTree(intersect, 0.025);
    int count = 0;
    for (const auto index : res) {
      _462::real_t dist = _462::distance(absorb_photons_[index].pos, intersect);
      _462::Vector3 color = absorb_photons_[index].power;
      if (dist >= 1.0f) {
        color /= dist;
      }
      _462::real_t dr1 = _462::dot(absorb_photons_[index].dir,
                                   absorb_photons_[index].pos - intersect);
      global += color;
      count++;
    }
    if (count) {
      global /= (_462::real_t)count;
      result = _462::Hadamard(direct, global);
    } else {
      result = _462::Vector3(0.0f, 0.0f, 0.0f);
    }
  } else {
    result = _462::Vector3(0.0f, 0.0f, 0.0f);
  }
  return result;
}

_462::Vector3 PhotonSimulator::GetRayColor(const int x, const int y,
                                           const int scene_length,
                                           const int scene_width,
                                           const _462::Vector3 &camera_pos,
                                           const _462::Vector3 &camera_ctr,
                                           const _462::Vector3 &camera_up) {
  _462::Vector3 dir = GetRayDir(x, y, scene_length, scene_width, camera_pos,
                                camera_ctr, camera_up);
  _462::Vector3 color = GetPixelColor(camera_pos, dir);
  return color;
}

_462::Vector3 PhotonSimulator::GetRayDir(const int x, const int y,
                                         const int scene_length,
                                         const int scene_width,
                                         const _462::Vector3 &camera_pos,
                                         const _462::Vector3 &camera_ctr,
                                         const _462::Vector3 &camera_up) const {
  _462::Vector3 dir = camera_ctr - camera_pos;
  _462::Vector3 cR = _462::cross(dir, camera_up);
  _462::Vector3 cU = _462::cross(cR, dir);
  _462::real_t AR = _462::length(cU) / _462::length(cR);
  _462::real_t dist = tan(kPI / 4) * 2 / _462::length(dir);
  _462::Vector3 t =
      dir + dist * (((_462::real_t)y - (_462::real_t)scene_width / 2) /
                        scene_width * cU +
                    AR * ((_462::real_t)x - (_462::real_t)scene_length / 2) /
                        scene_length * cR);
  t = normalize(t);
  return t;
}

PhotonSimulator::RadianceResult PhotonSimulator::RussianRoulette(
    const _462::real_t abr, const _462::real_t ref,
    const _462::real_t trans) const {
  _462::real_t a = (rand() % 100) / 100.0f;
  if (a < abr)
    return RadianceResult::kAbsorb;
  else if (a < abr + ref)
    return RadianceResult::kReflect;
  else
    return RadianceResult::kRefract;
}

std::tuple<Model *, Mesh *, Face *> PhotonSimulator::FindFirstIntersect(
    const _462::Vector3 &pos, const _462::Vector3 &dir) {
  Face *min_face = nullptr;
  Mesh *min_mesh = nullptr;
  Model *min_model = nullptr;
  _462::real_t min_distance = std::numeric_limits<double>::max();
  for (auto &model : models_) {
    Face *face = nullptr;
    Mesh *mesh = nullptr;
    _462::real_t distance = model.FindFirstIntersect(&face, &mesh, pos, dir);
    if (distance < min_distance) {
      min_face = face;
      min_mesh = mesh;
      min_model = &model;
    }
  }
  return std::make_tuple(min_model, min_mesh, min_face);
}

void PhotonSimulator::PhotonsModify() {
  while (!alive_photons_.empty()) {
    for (int i = 0; i < alive_photons_.size(); i++) {
      Face *min_face = nullptr;
      Mesh *min_mesh = nullptr;
      Model *min_model = nullptr;
      std::tie(min_model, min_mesh, min_face) =
          FindFirstIntersect(alive_photons_[i].pos, alive_photons_[i].dir);
      if (min_face != nullptr) {
        _462::Vector3 intersect = min_model->GetIntersect(
            *min_face, alive_photons_[i].pos, alive_photons_[i].dir);
        RadianceResult res = RussianRoulette(min_face->material.aborption,
                                             min_face->material.reflection,
                                             min_face->material.transmision);
        switch (res) {
          case RadianceResult::kAbsorb: {
            absorb_photons_.push_back(
                Photon(min_face->normal, intersect, alive_photons_[i].power));
            alive_photons_.erase(alive_photons_.begin() + i--);
            min_face->photons++;
            break;
          }
          case RadianceResult::kReflect: {
            _462::Vector3 ref =
                GetReflect(alive_photons_[i].dir, min_face->normal);
            alive_photons_[i].pos = intersect;
            alive_photons_[i].dir = ref;
            break;
          }
          case RadianceResult::kRefract: {
            _462::Vector3 ref =
                GetRefract(alive_photons_[i].dir, min_face->normal, 1.0);
            alive_photons_[i].pos = intersect;
            alive_photons_[i].dir = ref;
            break;
          }
          default:
            std::cout << "Error result!" << std::endl;
        }
      } else {
        alive_photons_.erase(alive_photons_.begin() + i--);
      }
    }
  }
  if (is_rendering_) {
    ConstructKDTree(absorb_photons_);
  }
}

_462::Vector3 PhotonSimulator::GetReflect(const _462::Vector3 &dir,
                                          const _462::Vector3 &norm) const {
  _462::Vector3 normal = normalize(norm);
  return (dir - 2 * _462::Hadamard((_462::Hadamard(dir, normal)), normal));
}

_462::Vector3 PhotonSimulator::GetRefract(const _462::Vector3 &dir,
                                          const _462::Vector3 &norm,
                                          _462::real_t coef) const {
  return _462::Vector3(
      -sqrt(1 - coef * coef *
                    (1 - _462::squared_length(_462::Hadamard(dir, norm)) *
                             _462::squared_length(_462::Hadamard(dir, norm)))) *
          norm +
      coef * (dir + _462::squared_length(_462::Hadamard(dir, norm)) * norm));
}

}  // namespace photonsimulator

}  // namespace simulator
