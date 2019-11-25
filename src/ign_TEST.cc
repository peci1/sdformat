/*
 * Copyright (C) 2017 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/

#include <gtest/gtest.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "sdf/parser.hh"
#include "sdf/SDFImpl.hh"
#include "sdf/sdf_config.h"
#include "test_config.h"

#ifdef _WIN32
  #define popen  _popen
  #define pclose _pclose
#endif

static const std::string g_sdfVersion(" --force-version " +
  std::string(SDF_VERSION_FULL));
static const std::string g_ignCommand(std::string(IGN_PATH) + "/ign");

/////////////////////////////////////////////////
std::string custom_exec_str(std::string _cmd)
{
  _cmd += " 2>&1";
  FILE *pipe = popen(_cmd.c_str(), "r");

  if (!pipe)
    return "ERROR";

  char buffer[128];
  std::string result = "";

  while (!feof(pipe))
  {
    if (fgets(buffer, 128, pipe) != NULL)
      result += buffer;
  }

  pclose(pipe);
  return result;
}

/////////////////////////////////////////////////
TEST(check, SDF)
{
  std::string pathBase = PROJECT_SOURCE_PATH;
  pathBase += "/test/sdf";

  // Check a good SDF file
  {
    std::string path = pathBase +"/box_plane_low_friction_test.world";

    // Check box_plane_low_friction_test.world
    std::string output =
      custom_exec_str(g_ignCommand + " sdf -k " + path + g_sdfVersion);
    EXPECT_EQ("Valid.\n", output) << output;
  }

  // Check a bad SDF file
  {
    std::string path = pathBase +"/box_bad_test.world";

    // Check box_bad_test.world
    std::string output =
      custom_exec_str(g_ignCommand + " sdf -k " + path + g_sdfVersion);
    EXPECT_NE(output.find("Required attribute"), std::string::npos)
      << output;
  }

  // Check an SDF file with sibling elements of the same type (world)
  // that have duplicate names.
  {
    std::string path = pathBase +"/world_duplicate.sdf";

    // Check world_duplicate.sdf
    std::string output =
      custom_exec_str(g_ignCommand + " sdf -k " + path + g_sdfVersion);
    EXPECT_NE(output.find("Error: World with name[default] already exists."),
              std::string::npos) << output;
  }

  // Check an SDF file with sibling elements of different types (model, light)
  // that have duplicate names.
  {
    std::string path = pathBase +"/world_sibling_same_names.sdf";

    // Check world_sibling_same_names.sdf
    std::string output =
      custom_exec_str(g_ignCommand + " sdf -k " + path + g_sdfVersion);
    EXPECT_NE(output.find("Error: non-unique names"), std::string::npos)
      << output;
  }

  // Check an SDF file with sibling elements of the same type (link)
  // that have duplicate names.
  {
    std::string path = pathBase +"/model_duplicate_links.sdf";

    // Check model_duplicate_links.sdf
    std::string output =
      custom_exec_str(g_ignCommand + " sdf -k " + path + g_sdfVersion);
    EXPECT_NE(output.find("Error: link with name[link] already exists."),
              std::string::npos) << output;
  }

  // Check an SDF file with sibling elements of the same type (joint)
  // that have duplicate names.
  {
    std::string path = pathBase +"/model_duplicate_joints.sdf";

    // Check model_duplicate_joints.sdf
    std::string output =
      custom_exec_str(g_ignCommand + " sdf -k " + path + g_sdfVersion);
    EXPECT_NE(output.find("Error: joint with name[joint] already exists."),
              std::string::npos) << output;
  }

  // Check an SDF file with sibling elements of different types (link, joint)
  // that have duplicate names.
  {
    std::string path = pathBase +"/model_link_joint_same_name.sdf";

    // Check model_link_joint_same_name.sdf
    std::string output =
      custom_exec_str(g_ignCommand + " sdf -k " + path + g_sdfVersion);
    EXPECT_NE(output.find("Error: non-unique names"), std::string::npos)
      << output;
  }

  // Check an SDF file with sibling elements of the same type (collision)
  // that have duplicate names.
  {
    std::string path = pathBase +"/link_duplicate_sibling_collisions.sdf";

    // Check link_duplicate_sibling_collisions.sdf
    std::string output =
      custom_exec_str(g_ignCommand + " sdf -k " + path + g_sdfVersion);
    EXPECT_NE(output.find("Error: collision with name[collision] "
                          "already exists."),
              std::string::npos) << output;
  }

  // Check an SDF file with sibling elements of the same type (visual)
  // that have duplicate names.
  {
    std::string path = pathBase +"/link_duplicate_sibling_visuals.sdf";

    // Check link_duplicate_sibling_visuals.sdf
    std::string output =
      custom_exec_str(g_ignCommand + " sdf -k " + path + g_sdfVersion);
    EXPECT_NE(output.find("Error: visual with name[visual] already exists."),
              std::string::npos) << output;
  }

  // Check an SDF file with cousin elements of the same type (collision)
  // that have duplicate names. This is a valid file.
  {
    std::string path = pathBase +"/link_duplicate_cousin_collisions.sdf";

    // Check link_duplicate_cousin_collisions.sdf
    std::string output =
      custom_exec_str(g_ignCommand + " sdf -k " + path + g_sdfVersion);
    EXPECT_EQ("Valid.\n", output) << output;
  }

  // Check an SDF file with cousin elements of the same type (visual)
  // that have duplicate names. This is a valid file.
  {
    std::string path = pathBase +"/link_duplicate_cousin_visuals.sdf";

    // Check link_duplicate_cousin_visuals.sdf
    std::string output =
      custom_exec_str(g_ignCommand + " sdf -k " + path + g_sdfVersion);
    EXPECT_EQ("Valid.\n", output) << output;
  }

  // Check an invalid SDF file that uses reserved names.
  {
    std::string path = pathBase +"/model_invalid_reserved_names.sdf";

    // Check model_invalid_reserved_names.sdf
    std::string output =
      custom_exec_str(g_ignCommand + " sdf -k " + path + g_sdfVersion);
    EXPECT_NE(output.find("Error: The supplied link name [world] is reserved."),
              std::string::npos) << output;
    EXPECT_NE(output.find("Error: The supplied link name [__link__] "
                          "is reserved."),
              std::string::npos) << output;
    EXPECT_NE(output.find("Error: The supplied visual name [__visual__] "
                          "is reserved."),
              std::string::npos) << output;
    EXPECT_NE(output.find("Error: The supplied collision name [__collision__] "
                          "is reserved."),
              std::string::npos) << output;
    EXPECT_NE(output.find("Error: The supplied joint name [__joint__] "
                          "is reserved."),
              std::string::npos) << output;
  }

  // Check that validity checks are disabled inside <plugin> elements
  {
    std::string path = pathBase +"/invalid_sdf_in_plugin.sdf";

    std::string output =
      custom_exec_str(g_ignCommand + " sdf -k " + path + g_sdfVersion);
    EXPECT_EQ("Valid.\n", output) << output;
  }

  // Check that validity checks are disabled inside namespaced elements
  {
    std::string path = pathBase +"/invalid_sdf_in_namespaced_elements.sdf";

    std::string output =
      custom_exec_str(g_ignCommand + " sdf -k " + path + g_sdfVersion);
    EXPECT_EQ("Valid.\n", output) << output;
  }
}

/////////////////////////////////////////////////
TEST(check_model_sdf, SDF)
{
  std::string pathBase = PROJECT_SOURCE_PATH;
  pathBase += "/test/integration/model/box";

  // Check a good SDF file by passing the absolute path
  {
    std::string path = pathBase +"/model.sdf";

    std::string output =
      custom_exec_str(g_ignCommand + " sdf -k " + path + g_sdfVersion);
    EXPECT_EQ("Valid.\n", output);
  }

  // Check a good SDF file from the same folder by passing a relative path
  {
    std::string path = "model.sdf";

    std::string output =
      custom_exec_str("cd " + pathBase + " && " +
                      g_ignCommand + " sdf -k " + path + g_sdfVersion);
    EXPECT_EQ("Valid.\n", output);
  }
}

/////////////////////////////////////////////////
TEST(describe, SDF)
{
  // Get the description
  std::string output =
    custom_exec_str(g_ignCommand + " sdf -d " + g_sdfVersion);
  EXPECT_FALSE(output.empty());

  // The first line should start with the following text.
  EXPECT_EQ(0u, output.find("<element name ='sdf' required ='1'"));
}

/////////////////////////////////////////////////
TEST(print, SDF)
{
  std::string pathBase = PROJECT_SOURCE_PATH;
  pathBase += "/test/sdf";

  // Check a good SDF file
  {
    std::string path = pathBase +"/box_plane_low_friction_test.world";
    sdf::SDFPtr sdf(new sdf::SDF());
    EXPECT_TRUE(sdf::init(sdf));
    EXPECT_TRUE(sdf::readFile(path, sdf));

    // Check box_plane_low_friction_test.world
    std::string output =
      custom_exec_str(g_ignCommand + " sdf -p " + path + g_sdfVersion);
    EXPECT_EQ(sdf->Root()->ToString(""), output);
  }

  // Check a bad SDF file
  {
    std::string path = pathBase +"/box_bad_test.world";

    // Check box_bad_test.world
    std::string output =
      custom_exec_str(g_ignCommand + " sdf -p " + path + g_sdfVersion);
    EXPECT_TRUE(output.find("Required attribute") != std::string::npos);
  }
}

/////////////////////////////////////////////////
/// Main
int main(int argc, char **argv)
{
  // Set IGN_CONFIG_PATH to the directory where the .yaml configuration file
  // is located.
  setenv("IGN_CONFIG_PATH", IGN_CONFIG_PATH, 1);

  // Make sure that we load the library recently built and not the one installed
  // in your system. This is done by placing the the current build directory
  // first in the LD_LIBRARY_PATH environment variable.
  //
  // We need to keep the existing LD_LIBRARY_PATH so that libsdformat.so can
  // find its dependency.
#ifndef _WIN32
  std::string testLibraryPath = IGN_TEST_LIBRARY_PATH;

  char *currentLibraryPath = std::getenv("LD_LIBRARY_PATH");
  if (currentLibraryPath)
  {
    testLibraryPath = testLibraryPath + ":" + currentLibraryPath;
  }

  setenv("LD_LIBRARY_PATH", testLibraryPath.c_str(), 1);
#endif

  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
