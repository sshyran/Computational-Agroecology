#include <utility>

#ifndef AGROECOLOGY_COMMON_PLANT_TYPE_H_
#define AGROECOLOGY_COMMON_PLANT_TYPE_H_

#include <string>

// Represents a single type of plant.

class PlantType {
 public:
  // TODO: Fix this class's format.

  PlantType(std::string name, int maxOT, int minOT, int maxAT, int minAT)
      : name_(std::move(name)),
        max_optimal_temperature_(maxOT),
        min_optimal_temperature_(minOT),
        max_absolute_temperature_(maxAT),
        min_absolute_temperature_(minAT) {}

  PlantType(std::string plantName, int maxOT, int minOT, int maxAT, int minAT,
            int maxAR, int minAR, int maxOR, int minOR, int* threshold,
            bool cultivar = false, int base = 0)
      : name_(plantName),
        max_optimal_temperature_(maxOT),
        min_optimal_temperature_(minOT),
        max_absolute_temperature_(maxAT),
        min_absolute_temperature_(minAT),
        max_absolute_annual_rainfall_(maxAR),
        min_absolute_annual_rainfall_(minAR),
        max_optimal_annual_rainfall_(maxOR),
        min_optimal_annual_rainfall_(minOR),
        cultivar_(cultivar),
        gdd_thresholds_(threshold),
        // std::array<int,5> gdd_thresholds_ = Thresholds,
        // //gdd_thresholds_(Thresholds),
        base_temp_(base) {}

  std::string name() { return name_; }
  int* gdd_thresholds() { return gdd_thresholds_; }
  int base_temp() { return base_temp_; }
  int max_optimal_temperature() { return max_optimal_temperature_; }
  int min_optimal_temperature() { return min_optimal_temperature_; }
  int max_absolute_temperature() { return max_absolute_temperature_; }
  int min_absolute_temperature() { return min_absolute_temperature_; }
  int max_absolute_annual_rainfall() { return max_absolute_annual_rainfall_; }
  int min_absolute_annual_rainfall() {
    return min_absolute_annual_rainfall_;
    ;
  }
  int max_optimal_annual_rainfall() { return max_optimal_annual_rainfall_; }
  int min_optimal_annual_rainfall() { return min_optimal_annual_rainfall_; }
  bool cultivar() { return cultivar_; }

 private:
  // the name of the single species or single cultivar
  std::string name_;

  int* gdd_thresholds_;

  int base_temp_;  // base temperature for Growing Degree Day calculations

  int max_optimal_temperature_;  // max optimal temperature requirement for
                                 // plant type

  int min_optimal_temperature_;  // min optimal temperature requirement for
                                 // plant type

  int max_absolute_temperature_;  // max absolute temperature requirement for
                                  // plant type

  int min_absolute_temperature_;  // min absolute temperature requirement for
                                  // plant type

  int max_absolute_annual_rainfall_;  // max absolute rainfall requirement for
                                      // plant type

  int min_absolute_annual_rainfall_;  // min absolute rainfall requirement for
                                      // plant type

  int max_optimal_annual_rainfall_;  // max optimal rainfall requirement for
                                     // plant type

  int min_optimal_annual_rainfall_;  // max optimal rainfall requirement for
                                     // plant type

  bool cultivar_;  //  specify whether this is a single species or single
                   //  cultivar.
};

#endif  // AGROECOLOGY_COMMON_PLANT_TYPE_H_
