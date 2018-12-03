#pragma once
#include "Engine/MathHelper.h"
// Note: I think the youngs modulus and mass are wrong. They should be modified for 2d. Ie: J/m^2 and kg/m^2
// As long as the values are relative its ok but requires tuning of the substeps to get the physically correct velocities.
// Planning to figure this out as I continue to simulate.

// Papers recommends 0.00001 for explicit integration.
static const GLfloat TIMESTEP = 0.001f;
static const GLfloat FLIP_PERCENT = 0.95f; // Percent to mix pic and flip velocities on the particles

// When outputting frames it's useful to take many simulation steps per frame (substeps)
// So we calculate the number of substeps from the desired FPS given the timestep
static const GLuint FPS = 30;
static const GLuint SUBSTEPS = static_cast<GLuint>((1.0f / TIMESTEP) / FPS); // Number of iterations required for 1s of simulation divi'd up among the number of frames every sec.
static const GLuint MAXFRAMES = 1000;

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

// When this is defined the program will compile to write frames
//#define OUTPUTFRAMES
// When this is defined the program will compile to write stats to the console every frame
//#define STATS