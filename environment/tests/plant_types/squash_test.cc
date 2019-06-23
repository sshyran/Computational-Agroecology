#include <string>

#include <gtest/gtest.h>

#include "plant_types/squash.h"

using namespace environment::plant_type;

const std::string kSquashTypeName = "Squash";

TEST(SquashTest, GlobalVariableTest) {
  const Squash& squash =
      *(reinterpret_cast<const Squash*>(plant_type_to_plant[kSquashTypeName]));

  EXPECT_EQ(kSquashTypeName, squash.type_name);
  EXPECT_EQ("-", squash.display_symbol);
  EXPECT_EQ(true, squash.cultivar);
  EXPECT_EQ(0.0, squash.base_temperature);
  EXPECT_EQ(MaxMinTemperature(0, 0), squash.optimal_temperature);
  EXPECT_EQ(MaxMinTemperature(0, 0), squash.absolute_temperature);
  EXPECT_EQ(MaxMinRainfall(0, 0), squash.optimal_annual_rainfall);
  EXPECT_EQ(MaxMinRainfall(0, 0), squash.absolute_annual_rainfall);
}

TEST(SquashTest, GenerateTest) {
  const Squash& squash =
      *(reinterpret_cast<const Squash*>(plant_type_to_plant[kSquashTypeName]));
  auto plant = squash.GeneratePlantInstance();

  EXPECT_EQ(kSquashTypeName, plant->type_name);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}