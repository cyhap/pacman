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
  * @file Navigator.cpp
  * @copyright 2019 Ari Kupferberg
  * @author Ari Kupfeberg
  * @date 12/7/2019
  * @brief This ROS dependent Class is for Navigation Control.
  */

#include <math.h>
#include <stdlib.h>
#include <memory>
#include <vector>
#include <algorithm>

#include "Navigator.hpp"
#include "Movement.hpp"
#include "Object.hpp"

#include "pacman/ObjPose.h"  // Our custom msg data
#include "pacman/VecPoses.h"  // Our custom msg vector
#include "pacman/NavPose.h"  // Our custom srv type
#include "ros/ros.h"
#include "sensor_msgs/LaserScan.h"
#include "move_base_msgs/MoveBaseAction.h"
#include "actionlib/client/simple_action_client.h"

// used for object deletion
#include "gazebo_msgs/DeleteModel.h"
#include "gazebo_msgs/GetModelState.h"
#include "gazebo_msgs/ModelStates.h"
#include "std_msgs/String.h"
#include "geometry_msgs/Twist.h"
#include "geometry_msgs/Pose.h"

Navigator::Navigator()
  : navStackStatus{ false }, allowImgCallback{ true } , sendGoal{ false } ,
          aclient("move_base", true) {
  // THIS IS A CUSTOM THROTTLE TOPIC, DONT FORGET TO ADD IT TO LAUNCH
  subClosestObj_ = n_.subscribe("/my_model_states", 1000,
    &Navigator::closestCallback, this);

  // Service client for deleting objects
  clientDelObj_ = n_.serviceClient<gazebo_msgs::DeleteModel>
    ("/gazebo/delete_model");

  // Service client from get model state
  clientGetPos_ = n_.serviceClient<gazebo_msgs::GetModelState>
    ("/gazebo/get_model_state");

  // Publisher for collected objects
  colObjPose_ = n_.advertise<geometry_msgs::Point>("listobjects", 1000);

  closestObject = "none";
  deleteOkay = false;
}

void Navigator::laserScanCallback(const sensor_msgs::LaserScan::ConstPtr& msg) {
  ROS_INFO_STREAM("Received LaserScan");
  std::vector<float> ranges = msg->ranges;
  // Replace all nans with max range to get the actual minimum
  std::replace_if(ranges.begin(), ranges.end(), isNan, msg->range_max);
  float minRange = *std::min_element(ranges.begin(), ranges.end());
  ROS_INFO_STREAM("The closest object is " << minRange << "(m) away.");
  movement->updateMinDist(minRange);
}

void Navigator::imgCallback(const pacman::VecPoses::ConstPtr& vecPoses) {
  //  -- Find the closest pose from the vector input
  double minY = 0;
  for (const auto &indpose : vecPoses->poses) {
    if (indpose.collect) {
      // Grab y coordinate of current pose
      double ypos = indpose.y;
      // Check y value of pose
      if (ypos > minY) {
        // reset minY to current Y
        minY = ypos;
        // set current pose to closest
        closestPose = indpose;
      }
    }
  }
}

bool Navigator::checkVisuals() {
  // Check State of Navigation Stack
//  if (aclient.getState().isDone())
//    navStackStatus = false;
//  else
//    navStackStatus = true;
  navStackStatus = false;
//  ROS_INFO_STREAM("Checked Visuals, output: " << navStackStatus);
  return navStackStatus;
}

void Navigator::closestCallback(const gazebo_msgs::ModelStates msg) {
  float closestDist = 1000.0;

  // Get the position of the Turtlebot
  gazebo_msgs::GetModelState srvPos;

  double x1, y1;
  srvPos.request.model_name = "mobile_base";
  if (clientGetPos_.call(srvPos)) {
    x1 = srvPos.response.pose.position.x;
    y1 = srvPos.response.pose.position.y;
  } else {
    ROS_ERROR_STREAM("Failed to obtain Turtlebot position");
    x1 = 0;
    y1 = 0;
  }

  // Find the object nearest to the Turtlebot
  int closest = 0;
  int lent = msg.pose.size();
  for (int i = 0; i < lent; i++) {
    double x2 = msg.pose[i].position.x;  // pose from sub
    double y2 = msg.pose[i].position.y;  // pose from sub
    double x = x1-x2;
    double y = y1-y2;
    double dist = sqrt(x * x + y * y);  // dist formula
    if (dist < 1) {
      if (dist < closestDist) {
        if (msg.name[i] == "mobile_base") {
          // skip mobile_base
        } else {
          if (msg.name[i] == "ground_plane") {
            // skip ground plane
          } else {
            std::string wallstr("wall");
            std::string ghostStr("blue");
            std::size_t found1 = msg.name[i].find(wallstr);
            if (found1 != std::string::npos) {
              // skip grey walls
            } else {
              auto found2 = msg.name[i].find(ghostStr);
              if (found2 != ghostStr.npos) {
              // skip blue ghosts
              } else {
                closestDist = dist;
                closest = i;
                closestObject = msg.name[closest];  // found closest
              }
            }
          }
        }
      }
    }
  }
  ROS_INFO_STREAM(closestObject);
}

void Navigator::deleteObject() {
  if (closestObject != "none" && deleteOkay) {
    // Make the delete service call
    gazebo_msgs::DeleteModel dmsrv;
    dmsrv.request.model_name = closestObject;

    if (clientDelObj_.call(dmsrv)) {
      // Object succesfully deleted
      ROS_INFO_STREAM("Object deleted");
      closestObject = "none";
      // send pose to objectlist
      // Publish at 10 Hz.
      ros::Rate loop_rate(10.0);
      // Get the position of the Turtlebot
      gazebo_msgs::GetModelState turtlePos;
      turtlePos.request.model_name = "mobile_base";
      clientGetPos_.call(turtlePos);
      geometry_msgs::Point obj = turtlePos.response.pose.position;
      // Publish pose
      colObjPose_.publish(obj);
    }
  }
}

void Navigator::setDelete() {
  deleteOkay = true;
  ROS_INFO_STREAM("This object is okay to delete");
}

void Navigator::resetDelete() {
  deleteOkay = false;
  ROS_INFO_STREAM("Do not delete this collision item");
}

geometry_msgs::Twist Navigator::navigate(bool clear_, int &turns) {
  if (clear_) {
    // Set center width range of image
    double midImgLeft = 280;
    double midImgright = 360;

    ROS_WARN_STREAM("Path is clear!");
    double xVal = closestPose.x;
    if (xVal > midImgright) {  // Object on the right
      // Turn right
      ROS_WARN_STREAM("Rigth Turn: [0.1,-0.5]");
      vels.linear.x = 0.1;
      vels.angular.z = -0.50;
    } else if (xVal < midImgLeft) {  // Object on the left
      // Turn left
      ROS_WARN_STREAM("Left Turn: [0.1,0.5]");
      vels.linear.x = 0.1;
      vels.angular.z = 0.50;
    } else {  // Object centered
      // Go straight
      ROS_WARN_STREAM("Straight Ahead: [0.5,0]");
      vels.linear.x = 0.50;
      vels.angular.z = 0.00;
    }
  } else {  // Object blocking path
    if (closestPose.collect) {  // Is a good object
      setDelete();
      deleteObject();
    } else {
      resetDelete();
      ROS_WARN_STREAM("Object in path! Turning right");
      vels.linear.x = 0.00;
      if (turns < 30)
        vels.angular.z = -0.50;
      else
         vels.angular.z = 0.50;
      ++turns;
      if (turns >= 60)
        turns = 0;
    }
  }
  return vels;
}
