#ifndef COMPUTATIONAL_AGROECOLOGY_ENVIRONMENT_ENVIRONMENT_H_
#define COMPUTATIONAL_AGROECOLOGY_ENVIRONMENT_ENVIRONMENT_H_

#include <chrono>
#include <iostream>
#include <vector>

#include "climate.h"
#include "config.h"
#include "simulators/actions/action.h"
#include "simulators/main_simulator.h"
#include "simulators/sun_simulator.h"
#include "terrain.h"
#include "weather.h"

namespace environment {

// The main data structure which stores most of the data about this environment
// TODO: Copy-on-Write
class Environment {
 public:
  Environment(const Config &config,
              const std::chrono::system_clock::time_point &time,
              const std::chrono::duration<int> &time_step_length,
              const Terrain &terrain);

  // Jump to a specified time_step in the timeline.
  void JumpToTimeStep(const int64_t time_step);

  // Jump forward a number of time_steps in the timeline.
  void JumpForwardTimeStep(const int64_t time_step_num);

  // This receives an action from either an agent or a human
  // telling the internal simulator to schedule a task
  void ReceiveAction(const simulator::action::Action *action);

  // This is identical to the member function above except that a list of
  // actions are received here
  void ReceiveActions(const simulator::action::ActionList &actions);

  // TODO: define it
  const int score() const;

  // Accessors
  inline const Config &config() const { return config_; }

  inline const Climate &climate() const { return climate_; }

  inline const std::chrono::system_clock::time_point &timestamp() const {
    return timestamp_;
  }

  inline const std::chrono::duration<int> &time_step_length() const {
    return time_step_length_;
  }

  inline const int64_t &time_step() const { return time_step_; }

  inline const SunInfo &sun_info() const { return sun_info_; }

  inline const Terrain &terrain() const { return terrain_; }

  inline const Weather &weather() const { return weather_; }

 private:
  friend std::ostream &operator<<(std::ostream &os, const Environment &env);
  friend class simulator::MainSimulator;
  friend class simulator::SunSimulator;

  // befriend with a list of actions
  friend class simulator::action::crop::Add;
  friend class simulator::action::crop::Remove;
  friend class simulator::action::crop::Harvest;
  friend class simulator::action::crop::Water;

  // This tells its internal simulator to simulate to the specified time
  void JumpToTime(const std::chrono::system_clock::time_point &time);

  Config config_;
  const Climate climate_;

  // the information of sun from the simulator
  SunInfo sun_info_;

  // Current time in this environment
  std::chrono::system_clock::time_point timestamp_;
  // The length of a time step
  const std::chrono::duration<int> time_step_length_;
  // Indicates the time step in which this environment is in
  int64_t time_step_;

  Terrain terrain_;
  Weather weather_;
  // TODO: define a class for light information

  simulator::MainSimulator main_simulator_;
};

std::ostream &operator<<(std::ostream &os, const Environment &env);

}  // namespace environment

#endif  // COMPUTATIONAL_AGROECOLOGY_ENVIRONMENT_ENVIRONMENT_H_