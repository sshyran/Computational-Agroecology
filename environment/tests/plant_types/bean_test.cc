#include <string>

#include <gtest/gtest.h>

#include "plant_types/bean.h"

using namespace environment::plant_type;

const std::string kBeanTypeName = "Bean";

TEST(BeanTest, GlobalVariableTest) {
  const Bean& bean =
      *(reinterpret_cast<const Bean*>(plant_type_to_plant[kBeanTypeName]));

  EXPECT_EQ(kBeanTypeName, bean.type_name);
  EXPECT_EQ("o", bean.display_symbol);
  EXPECT_EQ(true, bean.cultivar);
  EXPECT_EQ(0.0, bean.base_temperature);
  EXPECT_EQ(MaxMinTemperature(0, 0), bean.optimal_temperature);
  EXPECT_EQ(MaxMinTemperature(0, 0), bean.absolute_temperature);
  EXPECT_EQ(MaxMinRainfall(0, 0), bean.optimal_annual_rainfall);
  EXPECT_EQ(MaxMinRainfall(0, 0), bean.absolute_annual_rainfall);
}

TEST(BeanTest, GenerateTest) {
  const Bean& bean =
      *(reinterpret_cast<const Bean*>(plant_type_to_plant[kBeanTypeName]));
  auto plant = bean.GeneratePlantInstance();

  EXPECT_EQ(kBeanTypeName, plant->type_name);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}