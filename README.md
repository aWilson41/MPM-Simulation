# MPM-Simulation
Material Point Method Simulation. See "A Material Point Method For Snow Simulation" for details.

Paper: https://www.math.ucla.edu/~jteran/papers/SSCTS13.pdf
Tech Report: http://alexey.stomakhin.com/research/siggraph2013_tech_report.pdf

Requires: GLM, GLFW, GLEW, Eigen

I might replace eigen with something more lightweight soon as I'm only using its singular value decompisition.

Build with CMake using the provided CMakeLists.txt.

This includes my own graphics abstraction (I might give it it's own repo soon).

Things to note: It uses explicit integration which means the timestep has to be very small. The paper suggests 0.00001. It also kinda of depends on other parameters. IE: Particle count and size mainly.