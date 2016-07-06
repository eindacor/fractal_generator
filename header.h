#pragma once

#ifndef HEADER_H
#define HEADER_H

#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <list>
#include <chrono>
#include <iostream>
#include <time.h>
#include <boost/smart_ptr/shared_ptr.hpp>
#include "ogl_tools.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtx/string_cast.hpp>
#include <gtx/intersect.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/functional/hash.hpp>
#include "jeploot.h"

//for bitmap creation
#include <Windows.h>
#include <memory>

using jep::ogl_context;
using jep::ogl_camera;
using jep::key_handler;
using jep::text_handler;
using jep::texture_handler;
using jep::ogl_camera_free;
using jep::ogl_camera_flying;

using boost::shared_ptr;

using glm::mat4;
using glm::vec2;
using glm::vec3;
using glm::vec4;

typedef std::pair<bool, float> random_switch;

class matrix_creator;
class fractal_generator;

#endif