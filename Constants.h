#pragma once
#include "Engine/MathHelper.h"

static const GLfloat TIMESTEP = 0.01f;
static const GLfloat FLIP_PERCENT = 0.95f;

static const GLfloat POSSIONS_RATIO = 0.48f;
static const GLfloat YOUNGS_MODULUS = 10000.0f; // Pa .1 GN/m^2 -> 1e8 N/m^2 -> 10000 N/m
static const GLfloat BULK_MODULUS = YOUNGS_MODULUS * POSSIONS_RATIO / ((1.0f + POSSIONS_RATIO) * (1.0f - 2.0f * POSSIONS_RATIO));
static const GLfloat SHEAR_MODULUS = YOUNGS_MODULUS / (2.0f * (1.0f + POSSIONS_RATIO));

static const GLfloat PARTICLE_MASS = 95.28f; // 930 kg / m^3 -> 95.28 kg / m^2
static const GLfloat PARTICLE_DIAMETER = 0.5f; // meters, This controls particle count