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
  * @file ObjectListNode.cpp
  * @copyright 2019 Ari Kupferberg
  * @author Ari Kupfeberg
  * @date 11/24/2019
  * @brief This ROS Node is for wrapping the ObjectList Class
  */

#include "Object.hpp"
#include <ros/ros.h>
#include <geometry_msgs/Point.h>
#include <vector>

/**
*  @brief   This is the main function
*  @param	  argc for ROS
*  @param	  argv for ROS
*  @return	0 Exit status
*/

int main(int argc, char **argv) {
  ros::init(argc, argv, "pubpose");
  ros::NodeHandle nh;
  ros::Publisher pub = nh.advertise<geometry_msgs::Point>("listobjects",1000);

  // Set random Poses
  std::vector<double> xs{0, 1, 2, 3, 4, 5};
  std::vector<double> ys{0, 9, 7, 5, 3, 1};
  std::vector<double> zs{0, 2, 4, 6, 4, 2};
  
  // Loop at 2Hz until the node is shut down
  ros::Rate rate(2);
  int counter = 0;

  while(ros::ok()) {
    while(counter < 6) {
      geometry_msgs::Point msg;
      msg.x = xs[counter];
      msg.y = ys[counter];
      msg.z = zs[counter];

      pub.publish(msg);
      counter++;

      // Send a message to rosout with the details.
      ROS_INFO_STREAM("Sent Pose");

      // Give one-time control to ROS
      ros::spinOnce();

      // Wait until next iteration
      rate.sleep();
    }
  }
}