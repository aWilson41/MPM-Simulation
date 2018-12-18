#pragma once
#include "Engine/MathHelper.h"

// Papers recommends 0.00001 for explicit integration.
// Maximum amount of frames allowed to output, only used if output frames is on
static const GLuint NUMFRAMES = 1000;
#define OUTPUTFRAMES
//#define TIMER

static const GLuint FPS = 60;
static const GLuint SUBSTEPS = 15;
static const GLfloat TIMESCALE = 0.5f;
static const GLfloat TIMESTEP = TIMESCALE / (FPS * SUBSTEPS);

// The possions ratio kinda gives us a ratio of shear to bulk (stretch/compressional) resistance. The youngs modulus is just an overall scale applied.
// The bulk and shear modulus scale the energy contributed by shear and bulk deformation respectively (see energy density eq.).
// High shear: Particles can easily slide past each other.
// Low bulk: Particles can easily compress/stretch
// Conversly
// Low shear: Particles can't easily slide past each other (very rigid)
// High bulk: Particles can't eaisly compress/stretch (again very rigid)
static const GLfloat POSSIONS_RATIO = 0.45f;
static const GLfloat YOUNGS_MODULUS = 1.4e5f; // Pa = J/m^3.
static const GLfloat BULK_MODULUS = YOUNGS_MODULUS * POSSIONS_RATIO / ((1.0f + POSSIONS_RATIO) * (1.0f - 2.0f * POSSIONS_RATIO)); // lambda
static const GLfloat SHEAR_MODULUS = YOUNGS_MODULUS / (2.0f * (1.0f + POSSIONS_RATIO)); // mu

// When spawning particles we calculate the total area and do particle count = totalArea/particleArea. 
// Error should be carefully avoided here. The paper recommends about 4 particles per cell
// The density of snow really varies from 50 to 800 kg/m^3
// Over the course of the simulation this will change due to the properties of the snow. These parameters simply describe it in its initial state
static const GLfloat PARTICLE_MASS = 100.0f; // Using 400 kg/m^3 assuming 4 particles per cell 400/4=100 kg/m^3
static const GLfloat PARTICLE_DIAMETER = 0.03f; // This controls the particle count and in meters should be adjusted along with grid dim to get the desired density

// Note: The determinant of the deformation gradient gives the ratio of volumetric change
// When we elastically deform past the crit ratios we dump the elastic deformation into plastic.
// This becomes important because the energy given by deformation is exponentially increased with plastic deformation
// This helps particles move more freely when plastically deformed
// Ie: e^(hardening * (plastic deformation - 1)) is multiplied with both the shear and bulk terms of the energy density equation
static const GLfloat HARDENING = 10.0f;
static const GLfloat CRIT_STRETCH = 1.0f + 7.5e-3f;
static const GLfloat CRIT_COMPRESS = 1.0f - 1.9e-2f;

// Still working on collision but this is just a ratio multiplied with the tangential velocity during colllision
static const GLfloat FRICTION = 1.0f;

// Percent to mix pic and flip velocities on the particles
static const GLfloat FLIP_PERCENT = 0.95f;