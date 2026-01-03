<div align="center">

# 🌌 **Relativistic Black Hole Raytracer** 🌌
### **Real-time Geodesic Raytracing of Sagittarius A* using Schwarzschild Metric & GPU Compute Shaders**

https://github.com/shanmuckh/Black_Hole/blob/main/Black_Hole_Output.mp4
**Physically-accurate simulation of gravitational lensing, photon spheres, and ergosphere dynamics around a Kerr-like black hole.**
*OpenGL 4.3 • GLSL Compute • RK4 Geodesic Integration • 100k+ rays/frame*

</div>

## 🚀 **Features**
- **2D CPU Raytracer**: Euler → RK4 integration of null geodesics with trail visualization
- **3D GPU Raytracer**: 400×300 Schwarzschild lensing with **20k ray steps/frame** on compute shaders
- **Real Physics**: Event horizon (`r_s = 2GM/c²`), **Christoffel symbols** for curved spacetime, conserved **E/L** quantities
- **Accretion Disk**: Volumetric `r⁻²` density profile with relativistic beaming
- **Interactive**: Orbital camera, zoom/pan, real-time parameter tuning

## 🧮 **Physics Deep Dive**
- Null Geodesic Equation: d²x^μ/dλ² + Γ^μ_αβ (dx^α/dλ)(dx^β/dλ) = 0
- Schwarzschild Metric: ds² = -(1-rs/r)dt² + (1-rs/r)⁻¹dr² + r²dΩ²
- Photon Sphere: r = 1.5 r_s (unstable circular orbit)
