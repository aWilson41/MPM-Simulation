#pragma once
#include <MathHelper.h>

// Papers recommends 0.00001 for explicit integration.
// Maximum amount of frames allowed to output, only used if output frames is on
static const GLuint NUMFRAMES = 1000;
//#define OUTPUTFRAMES
//#define TIMER

// Ex: 60fps * 10substeps = 600 steps per second, 10 per frame
static const GLuint FPS = 60;
static const GLuint SUBSTEPS = 100;
static const GLfloat TIMESCALE = 1.0f;
static const GLfloat TIMESTEP = TIMESCALE / (FPS * SUBSTEPS);

// The possions ratio gives us a ratio of shear to bulk (stretch/compressional) resistance.
// The youngs modulus is an overall scale applied. Intrinsic to the material being simulated.
// High shear: Particles can easily slide past each other.
// Low bulk: Particles can easily compress/stretch.
static const GLfloat POSSIONS_RATIO = 0.4f;
static const GLfloat YOUNGS_MODULUS = 1.4e5f; // Pa = J/m^3.
static const GLfloat BULK_MODULUS = YOUNGS_MODULUS * POSSIONS_RATIO / ((1.0f + POSSIONS_RATIO) * (1.0f - 2.0f * POSSIONS_RATIO)); // lambda
static const GLfloat SHEAR_MODULUS = YOUNGS_MODULUS / (2.0f * (1.0f + POSSIONS_RATIO)); // mu

// When spawning particles we calculate the total area and do particle count = totalArea/particleArea.
// Error should be carefully avoided here. The paper recommends about 4 particles per cell
// The density of snow really varies from 50 to 800 kg/m^3
static const GLfloat PARTICLE_MASS = 100.0f; // Using 400 kg/m^3 assuming 4 particles per cell 400/4=100 kg/m^3
static const GLfloat PARTICLE_DIAMETER = 0.045f; // Controls particle count. Change this along with grid size to get desired density
static const UINT GRID_DIM = 32;

// Note: The determinant of the deformation gradient gives the ratio of volumetric change
// When we elastically deform past the crit ratios we dump the elastic deformation into plastic.
// This becomes important because the energy given by deformation is exponentially increased with plastic deformation
// This helps particles move more freely when plastically deformed
// Ie: e^(hardening * (plastic deformation - 1)) is multiplied with both the shear and bulk terms of the energy density equation
static const GLfloat HARDENING = 10.0f;
static const GLfloat CRIT_STRETCH = 1.0f + 7.5e-3f;
static const GLfloat CRIT_COMPRESS = 1.0f - 2.5e-2f;

// Still working on collision but this is just a ratio multiplied with the tangential velocity during colllision
static const GLfloat FRICTION = 0.2f;

// Percent to mix pic and flip velocities on the particles
static const GLfloat FLIP_PERCENT = 0.95f;