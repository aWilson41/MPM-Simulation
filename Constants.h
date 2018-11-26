#pragma once
#include "Engine/MathHelper.h"

static const GLfloat TIMESTEP = 0.00001f;
static const GLfloat FLIP_PERCENT = 0.95f; // Percent to mix pic and flip velocities on the particles

static const GLfloat POSSIONS_RATIO = 0.2f;// 0.48f;
static const GLfloat YOUNGS_MODULUS = 1.4e5f; // Pa, N/m^2, N/m
static const GLfloat BULK_MODULUS = YOUNGS_MODULUS * POSSIONS_RATIO / ((1.0f + POSSIONS_RATIO) * (1.0f - 2.0f * POSSIONS_RATIO)); // lambda
static const GLfloat SHEAR_MODULUS = YOUNGS_MODULUS / (2.0f * (1.0f + POSSIONS_RATIO)); // mu

static const GLfloat PARTICLE_MASS = 100.0f;// 95.28f; // 930 kg / m^3 -> 95.28 kg / m^2
static const GLfloat PARTICLE_DIAMETER = 0.04f; // Meters, This controls particle count

// Increase energy given by plastic deformation
static const GLfloat HARDENING = 10.0f;

// Exceeding the following threshold will cause plastic deformation
static const GLfloat CRIT_STRETCH = 1.0f + 7.5e-3f;
static const GLfloat CRIT_COMPRESS = 1.0f - 2.5e-2f;

static const GLfloat FRICTION = 1.0f;