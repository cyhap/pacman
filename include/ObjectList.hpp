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
  * @file ObjectList.hpp
  * @copyright 2019 Ari Kupferberg
  * @author Ari Kupfeberg
  * @date 11/24/2019
  * @brief This Class is for storing and listing the collected objects.
  */

#ifndef INCLUDE_OBJECTLIST_HPP_
#define INCLUDE_OBJECTLIST_HPP_

#include <vector>

#include "Object.hpp"
#include "GoodObject.hpp"

#include "geometry_msgs/Point.h"

class ObjectList {
 private:
    // Vector of collected objects
    std::vector<Object::Pose> objectsFound;
    // Number of objects in found list
    int numberOfObjects;

 public:
    // Boolean flag to check for object collected
    bool objectFlag;

    /**
    *  @brief   This is the constructor for the ObjectList Class
    *  @param	  None
    *  @return	None
    */
    ObjectList();

    /**
    *  @brief   This is the destructor for the ObjectList Class
    *  @param	  None
    *  @return	None
    */
    ~ObjectList();

    /**
    *  @brief   This is a function for adding an object to the list
    *  @param	  obj Pointer to an object found
    *  @return	The number of objects in the list
    */
    void addObjectFound(Object::Pose);

    /**
    *  @brief   This is the callback function for updating the object list
    *  @param	  obj Pointer to the object found
    *  @return	None
    */
    void objsCallback(const geometry_msgs::Point::ConstPtr&);

    /**
    *  @brief   This is a function to get the list of objects
    *  @param	  None
    *  @return	The vector of objects
    */
    std::vector<Object::Pose> getObjectList();

    /**
    *  @brief   This is a function to get the number of objects
    *  @param	  None
    *  @return	The number of objects
    */
    int getSize();
};

#endif  // INCLUDE_OBJECTLIST_HPP_
