#pragma once

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <fstream>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <iomanip>
#include <list>
#include <mutex>
#include <random>
#include <set>
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <streambuf>
#include <thread>
#include <vector>

#include <GL/glew.h>

// Cross platform window creation
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <glm/glm.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/noise.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

using glm::ivec2;
using glm::uvec2;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;
using glm::quat;

// Image loading 
#include <gli/gli.hpp>

#define ENTRY_POINT_START \
        int main(const int argc, const char *argv[]) {
#define ENTRY_POINT_END \
        }

#define RUN_EXAMPLE(ExampleType) \
    ENTRY_POINT_START \
        ExampleType().run(); \
    ENTRY_POINT_END
