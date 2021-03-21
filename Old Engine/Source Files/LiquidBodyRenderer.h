#pragma once

Shader* dusk_liquid_shader;
Shader* realistic_liquid_shader;
Shader* fluid_dynamics_accurate_shader;

Shader* currentLiquidShader;

class LiquidBodyRenderer
{
public:
	vector<LiquidBody*> liquids;

	void addLiquidBody(LiquidBody* liquid)
	{
		liquids.push_back(liquid);
	}

	void setupShaderUniforms(glm::mat4 projection_, glm::mat4 view_)
	{
		currentLiquidShader->use();
		currentLiquidShader->setMat4("projection", glm::mat4(projection_));
		currentLiquidShader->setMat4("view", glm::mat4(view_));
		currentLiquidShader->setFloat("currentTime", glfwGetTime());
	}

	glm::mat4 setupModelMatrix(int Index)
	{
		currentLiquidShader->use();
		glm::vec3 liqPos = glm::vec3(liquids.at(Index)->getX(), 0.75f , liquids.at(Index)->getZ());
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, liqPos);

		currentLiquidShader->setMat4("model", model);
		currentLiquidShader->setBool("isFlowing", liquids.at(Index)->isFlowing);
		currentLiquidShader->setVec3("flowDirection", liquids.at(Index)->flowDirection);

		currentLiquidShader->setFloat("tiling", liquids.at(Index)->tileFactor);
		currentLiquidShader->setFloat("waveSpeedFactor", liquids.at(Index)->waveSpeedFactor);
		currentLiquidShader->setFloat("alpha", liquids.at(Index)->alphaFactor);
		currentLiquidShader->setBool("additiveBlend", liquids.at(Index)->additive);



		return model;
	}

	void setupNormalMatrix(glm::mat4 modelMatrix)
	{
		currentLiquidShader->use();
		glm::mat3 normalMatrix = glm::mat3(1.0f);
		normalMatrix = glm::mat3(glm::transpose(glm::inverse(modelMatrix)));
		currentLiquidShader->setMat3("normalMatrix", normalMatrix);
	}


	Shader* getShader()
	{
		return currentLiquidShader;
	}


	void init(Shader* liquidShader1)
	{
		currentLiquidShader = liquidShader1;
	}

	void renderLiquids(glm::mat4 proj, glm::mat4 viewM, Camera camera2)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		setupShaderUniforms(proj, viewM);
		currentLiquidShader->setVec3("camPos", camera2.Position);

		for (int i = 0; i < liquids.size(); i++)
		{
			currentLiquidShader->use();
			
			if (liquids.at(i)->defaultMesh == true)
			{
				
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, liquids.at(i)->liquidTexture.textId);
				glBindVertexArray(liquids.at(i)->liquidMeshVAO);
				glm::mat4 model = setupModelMatrix(i);
				setupNormalMatrix(model);

				glDrawElements(GL_TRIANGLES, liquids.at(i)->INDEX_COUNT , GL_UNSIGNED_INT, 0);   //render
			}

			if (liquids.at(i)->modelMesh == true)
			{
				glm::mat4 model = setupModelMatrix(i);
				setupNormalMatrix(model);

				liquids.at(i)->liquidModel->noTextures = true;
				liquids.at(i)->liquidModel->Draw(currentLiquidShader);
			}

		}

		glDisable(GL_BLEND);
	}



private:

	void chooseShader(int index)
	{
		if (liquids.at(index)->generic_shader == true)
		{
			currentLiquidShader = dusk_liquid_shader;
		}

		if (liquids.at(index)->realistic_shader == true)
		{
			currentLiquidShader = realistic_liquid_shader;
		}

		if (liquids.at(index)->fluid_sim_shader == true)
		{
			currentLiquidShader = fluid_dynamics_accurate_shader;
		}
	}
};