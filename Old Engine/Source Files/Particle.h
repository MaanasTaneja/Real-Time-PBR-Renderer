#pragma once
class Particle
{
public:
	glm::vec3 velocity;
	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 color;

	float lifeLength;
	float rotation;
	float gravityEffect;

	float elapsedTime;
	
	float particleWorldGravity = -50.0f;

	int numOfRows;
	glm::vec2 texOffset1;
	glm::vec2 texOffset2;

	float blend;

	float currentRotation=0.0f;
	float currentScale = 0.0f;

	Particle(glm::vec3 vel, glm::vec3 pos ,float grav, glm::vec3 scal, float rot, float lifeLen)
	{
		velocity = vel;
		position = pos;
		gravityEffect = grav;
		scale = scal;
		rotation = rot;
		lifeLength = lifeLen;
	}

	bool updateParticle()
	{
		velocity.y += particleWorldGravity * gravityEffect * (deltaTime);
		
		glm::vec3 change = velocity;
	    change = change * deltaTime;
	
		position = change + position;

		elapsedTime += deltaTime;
		return elapsedTime > lifeLength ? false : true;

	}

	void updateTextureCoords()
	{
		float lifeFactor = elapsedTime / lifeLength;
		int stageCount = numOfRows * numOfRows;


		float atlasProgression = lifeFactor * stageCount;

		int index = (int) floor(atlasProgression);
		int index2 = index < stageCount-1 ? index+1 : index;
		blend = fmod(atlasProgression, 1);

		texOffset1 = setTextureOffsets(index);
		texOffset2 = setTextureOffsets(index2);

	}

	glm::vec2 setTextureOffsets( int index)
	{
		int column = index % numOfRows;
		int row = index / numOfRows;

		glm::vec2 offset;
		offset.x = column / numOfRows;
		offset.y = row / numOfRows;
		return offset;
	}

};