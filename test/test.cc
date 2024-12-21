#include <gtest/gtest.h>
#include "raylib.h"
#include "raymath.h"
#include "particle_filter.h"

// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions) {
  // Expect two strings not to be equal.
  EXPECT_STRNE("hello", "world");
  // Expect equality.
  EXPECT_EQ(7 * 6, 42);
}

TEST(vecnorm, vecnorm) {
  Vector2 vec = {3, 4};
  EXPECT_FLOAT_EQ(vecnorm(vec), 5);
}

TEST(normal_pdf, normal_pdf) {
  float distance1 = 50;
  float distance2 = 100;
  float sensor = 100;
  float sigma = 50;
  EXPECT_FLOAT_EQ(normal_pdf(distance1, sensor, sigma), 0.00483941449);
  EXPECT_FLOAT_EQ(normal_pdf(distance2, sensor, sigma), 0.007978845608);
}

//theoretically ranging from -limit to limit
//but in practice, it is ranging from -3 to 3
TEST(GetNormalRandomValue, GetNormalRandomValue) {
  for (size_t i = 0; i < PARTICLES_NUMBERS; i++) {
    float value = GetNormalRandomValue();
    EXPECT_GE(value, -3);
    EXPECT_LE(value, 3);
  }
}

//Init particles in the range of 0 to 6*MAX_X and 0 to 6*MAX_Y
TEST(init, init) {
  init(input_particles);
  for (size_t i = 0; i < PARTICLES_NUMBERS; i++) {
    EXPECT_GE(input_particles[i].x, 0);
    EXPECT_LE(input_particles[i].x, 6 * MAX_X);
    EXPECT_GE(input_particles[i].y, 0);
    EXPECT_LE(input_particles[i].y, 6 * MAX_Y);
  }
}

TEST(update_weights, update_weights) {
  float sensor[2] = {100, 100};
  Vector2 landmarks[2] = {{150, 100}, {200, 100}};
  //ARRANGE
  input_particles[0] = {150, 200}; //distance to first landmark is 100
  input_particles[1] = {200, 200}; //distance to second landmark is 100
  input_particles[2] = {175, 196.82458366}; //distance to two landmarks are 100
  //ACT
  update_weights(weights, input_particles, sensor, 2, landmarks, 2, 50.0f);
  //ASSERT
  EXPECT_FLOAT_EQ(weights[0], 0.33022308);
  EXPECT_FLOAT_EQ(weights[1], 0.33022308);
  EXPECT_FLOAT_EQ(weights[2], 0.33955383); // get the highest weight
}

TEST(uniform_random, uniform_random) {
  for (size_t i = 0; i < 1000; i++) {
    float value = uniform_random();
    EXPECT_GE(value, 0);
    EXPECT_LT(value, 1);
  }
}

TEST(systematic_resample, systematic_resample) {
  //ARRANGE
  weights[0] = 0.01;
  weights[1] = 0.50;
  weights[2] = 0.49;
  size_t num = 3;
  //ACT
  systematic_resample(output_indexes, weights, num);
  //ASSERT
  for (size_t i = 0; i < num; i++) {
    EXPECT_GE(output_indexes[i], 0);
    EXPECT_LE(output_indexes[i], num - 1);
  }

  EXPECT_EQ(output_indexes[0], 1);
  EXPECT_EQ(output_indexes[1], 2);
  EXPECT_EQ(output_indexes[2], 2);
}