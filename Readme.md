<div align="center">

# ﴾⦵﴿ **Black Hole** ﴾⦵﴿

https://github.com/user-attachments/assets/371f3d6a-80b3-4fd2-8caa-ee46efdf2963

**Physically-accurate simulation of gravitational lensing, photon spheres, and ergosphere dynamics around a Kerr-like black hole.**
*OpenGL 4.3 • GLSL Compute • RK4 Geodesic Integration*

</div>

##  **Features**
- **2D CPU Raytracer**: Euler → RK4 integration of null geodesics with trail visualization
- **3D GPU Raytracer**: 400×300 Schwarzschild lensing with **20k ray steps/frame** on compute shaders
- **Real Physics**: Event horizon (`r_s = 2GM/c²`), **Christoffel symbols** for curved spacetime, conserved **E/L** quantities
- **Accretion Disk**: Volumetric `r⁻²` density profile with relativistic beaming
- **Interactive**: Orbital camera, zoom/pan, real-time parameter tuning

## **Now the Fun Part Begins 😎**
 **Schwarzschild Physics**
- Metric: ds² = -(1-rs/r)dt² + (1-rs/r)⁻¹dr² + r²(dθ² + sin²θ dφ²)
- Null Geodesic: g_μν dx^μ/dλ dx^ν/dλ = 0

- Christoffel Symbols (key):
- Γ^φ_rφ = Γ^φ_φr = -1/r
- Γ^r_tt = (rs/2r²)(1-rs/r)
- Γ^r_r r = -rs/2r²(1-rs/r)⁻¹

- Event Horizon: r_s = 2GM/c² = 2.53e10m (Sag A*)
- Photon Sphere: r = 1.5 r_s (unstable)

## **Hey there! 👋**
This is my Research and Development work - crafted with love for physics + graphics programming!

- ⭐ Star if you found this cool
- 👀 Follow for more simulations
- 🐛 Issues/PRs welcome—let's make it better together!

© <script>document.write(new Date().getFullYear())</script> [<a href="https://github.com/shanmuckh">Surakarapu Shanmukh Srinivas</a>]. All rights reserved. MIT License.
