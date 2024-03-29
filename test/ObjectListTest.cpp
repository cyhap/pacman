/*
 * Copyright (c) 2019, Ari Kupferberg, Ethan Quist, Corbyn Yhap
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 /**
  * @file ObjectListTest.cpp
  * @copyright 2019 Ari Kupferberg
  * @author Ari Kupfeberg
  * @date 11/26/2019
  * @brief This TEST file is for testing the ObjectList methods.
  */

#include "ObjectList.hpp"
#include "Object.hpp"

#include "ros/ros.h"
#include "gtest/gtest.h"


TEST(ObjectList, addObject) {
  ObjectList objList;

  Object::Pose locData;
  objList.addObjectFound(locData);

  ASSERT_EQ(objList.getSize(), 1);
}

TEST(ObjectList, objcallback) {
  ObjectList objList;

  geometry_msgs::Point objPose;
  objPose.x = 1.00;
  objPose.y = 2.00;
  objPose.z = 3.00;
  geometry_msgs::Point::ConstPtr objPointer(new geometry_msgs::Point(objPose));
  objList.objsCallback(objPointer);

  std::vector<Object::Pose> list = objList.getObjectList();

  EXPECT_EQ(list[0].x, 1.00);
  EXPECT_EQ(list[0].y, 2.00);
  EXPECT_EQ(list[0].z, 3.00);
}


// Run all the tests that were declared with TEST()
int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  ros::init(argc, argv, "objectlist_test");
  ros::NodeHandle nh;
  return RUN_ALL_TESTS();
}
