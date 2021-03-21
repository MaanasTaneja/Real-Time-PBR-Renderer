#pragma once
Shader* particleShader;
class ParticleRenderer
{
public:
	vector<Particle*> particles;
	float gravityOfSystem = -50.0f;
	float sizeOfParticle;

	Texture* particleTexture;

	vector<Texture*> randomParticleTextures;


	int nRows;

	bool randomizeColors = false;
	bool hasATexture = false;

	float animateAllRot = 0.0f;
	float animateAllScale = 0.0f;
	glm::vec3 setForceColor = glm::vec3(0.0f);

	void addParticle(Particle* particle)
	{
		//particle->scale = glm::vec3(sizeOfParticle);
		particle->numOfRows = nRows;
		particle->particleWorldGravity = gravityOfSystem;
		particle->currentScale = sizeOfParticle;


		float r = (float)rand() / (float)RAND_MAX * 2.0f - 1.0f;
		float g = (float)rand() / (float)RAND_MAX * 2.0f - 1.0f;
		float b = (float)rand() / (float)RAND_MAX * 2.0f - 1.0f;

		particle->color = glm::vec3(r * 200, g * 100, b * 100);

		

		particles.push_back(particle);
	}

	void setupShaderUniforms(glm::mat4 projection_, glm::mat4 view_)
	{
		particleShader->use();
		particleShader->setMat4("projection", glm::mat4(projection_));
		particleShader->setMat4("view", glm::mat4(view_));
		particleShader->setBool("randomizeColors", randomizeColors);
		
		if (setForceColor != glm::vec3(0.0f))
		{
			particleShader->setVec3("setColor", setForceColor);
			particleShader->setBool("hasForceColor", true);
		
		}
		else {
			particleShader->setVec3("setColor", glm::vec3(1.0f));
			particleShader->setBool("hasForceColor", false);
		}
		if (hasATexture == true)
		{
			particleShader->setBool("hasTexture", true);
		}
	}


	glm::mat4 setupModelMatrix(glm::mat4 viewM, int Index, Camera cam)
	{
	
		glm::mat3 viewRot = glm::inverse(glm::mat3(viewM));
		glm::quat camRot = glm::quat_cast(viewRot);
		
		
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix ,particles.at(Index)->position);
		modelMatrix = modelMatrix * glm::mat4_cast(camRot);

		float deltaScale = animateAllScale - particles.at(Index)->scale.x;
		float factor_scale;
		factor_scale = deltaScale / particles.at(Index)->lifeLength;

		if (animateAllRot == 0.0f)
		{
			modelMatrix = glm::rotate(modelMatrix, glm::radians(particles.at(Index)->rotation), glm::vec3(0.0f, 0.0f, 1.0f));
		}
		else {
		
			modelMatrix = glm::rotate(modelMatrix, glm::radians(particles.at(Index)->currentRotation), glm::vec3(0.0f, 0.0f, 1.0f));
		}
		
		if (animateAllScale == 0.0f)
		{
			modelMatrix = glm::scale(modelMatrix, particles.at(Index)->scale);
		}
		else {
			modelMatrix = glm::scale(modelMatrix, glm::vec3(particles.at(Index)->currentScale));
		}

		
	
		particles.at(Index)->currentRotation = particles.at(Index)->currentRotation +1.5f;
		particles.at(Index)->currentScale = particles.at(Index)->currentScale - 0.01f > 0.0f ? particles.at(Index)->currentScale - 0.02f : 0.0f;
		//particles.at(Index)->currentScale = particles.at(Index)->currentScale == animateAllRot ? animateAllRot : particles.at(Index)->currentScale - 0.1f;

		particleShader->use();
		particleShader->setMat4("model", modelMatrix);

		if (randomizeColors == true)
		{
			particleShader->setVec3("randomColor", particles.at(Index)->color);
		}

		return modelMatrix;
	}

	void setupNormalMatrix(glm::mat4 modelMatrix)
	{
		particleShader->use();
		glm::mat3 normalMatrix = glm::mat3(1.0f);
		normalMatrix = glm::mat3(glm::transpose(glm::inverse(modelMatrix)));

		particleShader->setMat3("normalMatrix", normalMatrix);
	}

	void init(Shader* entShader)
	{
		particleShader = entShader; //change this to init
		//particleShader->setInt("particleTexture", 0);
	}

	Shader* getShader()
	{
		return particleShader;
	}

	void renderParticles(glm::mat4 proj, glm::mat4 viewM, Camera camera2)
	{
		prepareQuad();
		glEnable(GL_BLEND);
		glDisable(GL_CULL_FACE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		setupShaderUniforms(proj, viewM);

		glActiveTexture(GL_TEXTURE0);
		if (particleTexture != nullptr)
		{
				glBindTexture(GL_TEXTURE_2D, particleTexture->textId);
			
		}
		else {
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		for (int i = 0; i < particles.size(); i++)
		{
			glm::mat4 pass= setupModelMatrix(viewM, i, camera2);
			setupNormalMatrix(pass);
			particleShader->setVec2("textureOffset1", particles.at(i)->texOffset1);
			particleShader->setVec2("textureOffset2", particles.at(i)->texOffset1);
			particleShader->setVec2("texCoordInfo", glm::vec2(nRows, particles.at(i)->blend));
			glDrawArrays(GL_TRIANGLES, 0, 6);
			
		}

		glEnable(GL_CULL_FACE);

		for (int j = 0; j < particles.size(); j++)
		{
			updateParticleArray(j);
		}

	}

	void updateParticleArray(int index)
	{
		bool isAlive = particles.at(index)->updateParticle();
		if (isAlive == false)
		{
			particles = findAndRemove(particles.at(index), particles);
		}
	}

private:
	float vertices[100] = {
		// back face
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
	};


	unsigned int indices[6] = {  // note that we start from 0!
	0, 1, 3,   // first triangle
	1, 2, 3    // second triangle
	};

	unsigned int VAO;

	void prepareQuad()
	{
		if (VAO == 0)
		{
			unsigned int VBO, EBO;
			glGenBuffers(1, &VBO);


			glGenVertexArrays(1, &VAO);
			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);

			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);

			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);

			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
			glEnableVertexAttribArray(2);
			
		}

		glBindVertexArray(VAO);
	}

	vector<Particle*> findAndRemove(Particle* entity, vector<Particle*> list)
	{
		for (int i = 0; i < list.size(); i++)
		{
			if (entity == list.at(i))
			{
				list.erase(remove(list.begin(), list.end(), entity), list.end());
			}
		}

		return list;
	}


};