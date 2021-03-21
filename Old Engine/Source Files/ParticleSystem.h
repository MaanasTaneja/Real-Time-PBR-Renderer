#pragma once

#define SPHERE_SYSTEM 0
#define POINT_SYSTEM 1
#define LINE_SYSTEM 2	
#define TRAIL_SYSTEM 3
#define BURST_SYSTEM 4
#define FIRE_SYSTEM 5
#define LINE_SYSTEM 6
#define MUZZLE_FLASH_SYSTEM 8
#define STATIONARY_SPHERE_SYSTEM 9
#define ATMOSPHERIC_PARTICLES 10
#define EXPERIMENTAL_ENTITY_VERTEX_SYSTEM 7

class ParticleSystem
{
public:

	bool additive;
	bool looping = true;
	bool randomizeRot;
	bool randomizeScale;
	bool randomizeColor = false;
	bool hasCustomInitialVel = false;
	unsigned int textureAtlas;
	unsigned int systemType = 0;

	glm::vec3 systemPosition;
	glm::vec3 direction;
	glm::vec3 color;
	glm::vec3 scale = glm::vec3(0.06f);
	glm::vec3 sphereRadius = glm::vec3(5.0f);

	glm::vec3 linePO1;
	glm::vec3 linePO2;

	glm::vec3 customInitialVelocity = glm::vec3(0.0f);

	float gravityCompliment;
	float lifeLength;
	float partsPerSec;
	float Speed;

	Entity* trailEntity;
	
	int EntityListIndex = 0;
	ParticleRenderer particleRenderer;

	Texture* systemTextureAtlas;

	Camera* playerCam;

	ParticleSystem(unsigned int emitterType, glm::vec3 emitterCentre, glm::vec3 particleColor, glm::vec3 scale_, float pps , float gravEffect, float lifeLen, float speed,  bool loop)
	{
	
		systemType = emitterType;
		systemPosition = emitterCentre;
		partsPerSec = pps;
		gravityCompliment = gravEffect;
		lifeLength = lifeLen;
		looping = loop;
		Speed = speed;
		scale = scale_;

		particleRenderer.sizeOfParticle = scale.x;
		

		if (emitterType == BURST_SYSTEM)
		{
			looping = false;
			particleRenderer.sizeOfParticle = 0.2f;
			particleRenderer.gravityOfSystem = 0.5f;
		}

		if (emitterType == FIRE_SYSTEM)
		{
			particleRenderer.gravityOfSystem = 0.1f;
			
		}

		if (emitterType == MUZZLE_FLASH_SYSTEM)
		{
			looping = false;
		}

	

	}

	ParticleSystem(unsigned int emitterType, glm::vec3 emitterCentre, glm::vec3 initialVelocity, glm::vec3 scale_, float pps, float gravEffect, float lifeLen, float speed)
	{

		systemType = emitterType;
		systemPosition = emitterCentre;
		partsPerSec = pps;
		gravityCompliment = gravEffect;
		lifeLength = lifeLen;
		looping = false;
		Speed = speed;
		scale = scale_;

		particleRenderer.sizeOfParticle = scale.x;


		if (emitterType == BURST_SYSTEM)
		{
			looping = false;
			particleRenderer.sizeOfParticle = 0.2f;
			particleRenderer.gravityOfSystem = 0.5f;
		}

		if (emitterType == FIRE_SYSTEM)
		{
			particleRenderer.gravityOfSystem = 0.1f;

		}

		if (emitterType == MUZZLE_FLASH_SYSTEM)
		{
			looping = false;
		}

		customInitialVelocity = initialVelocity;

	}

	ParticleSystem(unsigned int emitterType, glm::vec3 linePos1, glm::vec3 linePos2,  glm::vec3 particleColor, glm::vec3 scale_, float pps, float gravEffect, float lifeLen, float speed, bool loop)
	{

		systemType = emitterType;
		linePO1 = linePos1;
		linePO2 = linePos2;

		partsPerSec = pps;
		gravityCompliment = gravEffect;
		lifeLength = lifeLen;
		looping = loop;
		Speed = speed;
		scale = scale_;

		particleRenderer.sizeOfParticle = scale.x;
		
		
	}


	ParticleSystem(unsigned int emitterType, Entity* entity, glm::vec3 particleColor, glm::vec3 scale_, float pps, float gravEffect, float lifeLen, float speed, bool loop)
	{

		systemType = emitterType;
		partsPerSec = pps;
		gravityCompliment = gravEffect;
		lifeLength = lifeLen;
		looping = loop;
		Speed = speed;
		scale = scale_;

		particleRenderer.sizeOfParticle = scale.x;
		trailEntity = entity;

	}

	ParticleSystem(Camera* PlayerCam, int pps, float speed, glm::vec3 scal, float lifelen, bool Atmospheric)
	{
		playerCam = PlayerCam;
		systemType = ATMOSPHERIC_PARTICLES;
		partsPerSec = pps;
		scale = scal;
		Speed = speed;
		lifeLength = lifelen;
	}

	
	void addTextureAtlas(string texName, int numOfrows)
	{
		systemTextureAtlas = new Texture();
		systemTextureAtlas->getTexID(texName);
		particleRenderer.nRows = numOfrows;
		particleRenderer.particleTexture = systemTextureAtlas;
		particleRenderer.hasATexture = true;
	}

	void randomizeParticleRotations()
	{
		randomizeRot = true;
	}

	void randomizeParticleScale()
	{
		randomizeScale = true;
	}

	void randomizeColors()
	{
		randomizeColor = true;
		particleRenderer.randomizeColors = true;
	}

	void setColor(glm::vec3 col)
	{
		particleRenderer.setForceColor = col;
	}

	void animateRotationTo(float rotation)
	{
		particleRenderer.animateAllRot = rotation;
	}

	void animateScaleTo(float animateScaleTo)
	{
		particleRenderer.animateAllScale = animateScaleTo;
	}

	
	void initSystem(Shader* particleShad)
	{
		particleRenderer.init(particleShad);
	}


	void generateParticles()			
	{
			//Particle(glm::vec3 vel, glm::vec3 pos ,float grav, glm::vec3 scal, float rot, float lifeLen)
			float numParticles = partsPerSec * deltaTime;
			int count = (int)floor(numParticles);
			float partialParticle = fmod(numParticles, 1);

			if (systemType == BURST_SYSTEM)
			{
				count = partsPerSec*3;
			}

			if (systemType == MUZZLE_FLASH_SYSTEM)
			{
				count = 1;
			}

			if (deltaTime > 0.25)
			{
				count -= 2;
			}
			
			if (looping == false)
			{
				count = partsPerSec;
			}


			for (int i = 0; i < count; i++)
			{
				emitParticle();
			}

			if ((float) rand() / RAND_MAX < partialParticle)
			{
				emitParticle();
			}

	}

	void emitParticle()
	{
		glm::vec3 vel;
		glm::vec3 currentParticlePos;
		glm::vec3 currentParticleScale = scale;

		float currentParticleRotation = 0.0f;
		float currentParticleLifeLen;
		
		float dirX = (float)(rand()) / float((RAND_MAX)) * 2.0f -1.0f;
		float dirZ = (float)(rand()) / float((RAND_MAX)) * 2.0f - 1.0f;

		 vel = glm::vec3(dirX, 10.0f, dirZ);
		 vel = glm::normalize(vel);
		vel = vel * Speed;

		currentParticlePos = systemPosition;
		currentParticleLifeLen = lifeLength;

		if (systemType == SPHERE_SYSTEM)
		{
		    vel = glm::ballRand(sphereRadius.x);
			vel = glm::normalize(vel);
			currentParticlePos += vel;
			vel = vel * Speed;
			
		}

		if (systemType == BURST_SYSTEM)
		{
			vel = glm::ballRand(sphereRadius.x);
			vel = glm::normalize(vel);
			//currentParticlePos += vel*0.1f;
			vel *= Speed;
		}

		if (systemType == FIRE_SYSTEM)
		{
			vel.x = vel.x / Speed;
			vel.y = vel.y / Speed;
		    vel.z = vel.z / Speed;

			currentParticlePos.x = currentParticlePos.x + vel.x*(Speed)*2.0f;
			currentParticlePos.z = currentParticlePos.z + vel.z * (Speed) * 2.0f;

			float random = (float)rand() / RAND_MAX * lifeLength;
			currentParticleLifeLen = random;
		}

		if (systemType == LINE_SYSTEM)
		{
			float t = (float)rand() / RAND_MAX * 2.0f - 1.0f;
			currentParticlePos = glm::mix(linePO1, linePO2, t);
		}

		if (systemType == MUZZLE_FLASH_SYSTEM)
		{
			currentParticleLifeLen = 0.1f;
		}

		if (systemType == STATIONARY_SPHERE_SYSTEM)
		{
			currentParticlePos += glm::ballRand(sphereRadius.x);
			vel = glm::vec3(0.0f);
			gravityCompliment = 0.0f;
		}

		if (systemType == ATMOSPHERIC_PARTICLES)
		{
			glm::vec3 tempPos = glm::ballRand(1.0f);
			currentParticlePos = playerCam->Position;
			currentParticlePos += tempPos;
			vel = glm::vec3(0.0f, 1.0f, 0.0f);
			vel *= Speed;

			gravityCompliment = 0.0f;
		}

		if (randomizeRot == true)
		{
			currentParticleRotation = (float)rand() / RAND_MAX * 45.0f - 1.0f;
		}

		if (randomizeScale == true)
		{
			float scale1 = (float)rand() / RAND_MAX * 35.0f - 1.0f;	
		}

		if (customInitialVelocity != glm::vec3(0.0f))
		{
		
			vel *= (customInitialVelocity);
		
		}
		
		Particle* currentSpawn = new Particle(vel, currentParticlePos, gravityCompliment, currentParticleScale, currentParticleRotation, currentParticleLifeLen);
		currentSpawn->currentRotation = currentParticleRotation;

		particleRenderer.addParticle(currentSpawn);
		
	}

	int i = 0;
	void render(glm::mat4 project, glm::mat4 view, Camera cam)
	{
		if (i == 0)
		{
			generateParticles();
		}

		if (looping == false)
		{
			i++;
		}


		for (int j = 0; j < particleRenderer.particles.size(); j++)
		{
			particleRenderer.updateParticleArray(j);
		
		}

		particleRenderer.renderParticles(project, view, cam);
	}


private:

	void emitBurstParticle()
	{

	}
	
};