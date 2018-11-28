# MPM-Simulation
Material Point Method Simulation. See "A Material Point Method For Snow Simulation" for details.

Paper: https://www.math.ucla.edu/~jteran/papers/SSCTS13.pdf
Tech Report: http://alexey.stomakhin.com/research/siggraph2013_tech_report.pdf

Requires: GLM, GLFW, GLEW, Eigen

I might replace eigen with something more lightweight soon as I'm only using its singular value decompisition.

Build with CMake using the provided CMakeLists.txt.

This includes my own graphics abstraction (I might give it it's own repo soon) includes LodePNG for reading/writing png images.