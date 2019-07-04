// ======================================================================== //
// Copyright 2009-2019 Intel Corporation                                    //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //
#pragma once

#if defined(EMBREE_DPCPP_SUPPORT)
#define CL_TARGET_OPENCL_VERSION 220
#define SYCL_SIMPLE_SWIZZLES
#include <CL/sycl.hpp>

#ifdef __SYCL_DEVICE_ONLY__
#define GLOBAL __global
#define LOCAL  __local

extern int   work_group_reduce_add(int x);
extern float work_group_reduce_min(float x);
extern float work_group_reduce_max(float x);

extern float atomic_min(volatile __global float *p, float val);
extern float atomic_min(volatile __local  float *p, float val);
extern float atomic_max(volatile __global float *p, float val);
extern float atomic_max(volatile __local  float *p, float val);

#else
#define GLOBAL 
#define LOCAL 

/* dummy functions for host */
inline int   work_group_reduce_add(int x) { return x; }
inline float work_group_reduce_min(float x) { return x; }
inline float work_group_reduce_max(float x) { return x; }

inline float atomic_min(volatile float *p, float val) { return val; };
inline float atomic_max(volatile float *p, float val) { return val; };

#endif

constexpr cl::sycl::access::target sycl_local    = cl::sycl::access::target::local;
constexpr cl::sycl::access::mode sycl_read_write = cl::sycl::access::mode::read_write;
constexpr cl::sycl::access::mode sycl_read       = cl::sycl::access::mode::read;
constexpr cl::sycl::access::mode sycl_write      = cl::sycl::access::mode::write;

namespace embree
{
  namespace gpu
  {

    inline float halfarea(const cl::sycl::float3 &d)
    {
      return fma((float)d.x(),((float)d.y()+(float)d.z()),(float)d.y()*(float)d.z());
    }

    inline float halfarea(const cl::sycl::float4 &d)
    {
      return fma((float)d.x(),((float)d.y()+(float)d.z()),(float)d.y()*(float)d.z());
    }
    
    inline float area(const cl::sycl::float3 &d)
    {
      return halfarea(d) * 2.0f;
    }

    inline float area(const cl::sycl::float4 &d)
    {
      return halfarea(d) * 2.0f;
    }
    
    template<typename T, cl::sycl::access::address_space space>
      static inline uint atomic_add(T *dest, const T count=1)
      {
	cl::sycl::multi_ptr<T,space> ptr(dest);
	cl::sycl::atomic<T,space> counter(ptr);
	return atomic_fetch_add(counter,count);      
      }

    template<typename T>
      static inline uint as_uint(T t)
      {
	const uint &t_ref = reinterpret_cast<const uint&>(t);	
	return t_ref;
      }

    template<typename T>
      static inline float as_float(T t)
      {
	const float &t_ref = reinterpret_cast<const float&>(t);	
	return t_ref;
      }
    
  };
};


#endif