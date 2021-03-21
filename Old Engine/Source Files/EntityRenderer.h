#pragma once
#include <vector>

Shader *staticShader;


class EntityRenderer
{

public:
	
	int nFrames = 0;
	vector<Entity*> ents;
	Texture* test;


	unsigned int irradiance;
	
	void addEntity(Entity *Ent)
	{
		Ent->tempCheck = "lol";
		ents.push_back(Ent);
		//cout << "ENTITY_RENDERER::Entity [VAO] "<<Ent->getVAO()<<" to render at" << " " << Ent->position.x << ", " << Ent->position.y << ", " << Ent->position.z << endl;
	}

	void setupShaderUniforms(glm::mat4 projection_, glm::mat4 view_ , Camera camera3) //temp to be shifted to masterrednerer
	{
		
		staticShader->use();
		staticShader->setMat4("projection", glm::mat4(projection_));
		staticShader->setMat4("view", glm::mat4(view_));
		staticShader->setVec3("skyColor", glm::vec3(1.0f, 1.0f, 1.0f));
		
	}

	glm::mat4 setupModelMatrix(int Index)  //SETS UP ONLY MODEL BOI
	{
		staticShader->use();
		btTransform physicsTransform;
		 ents.at(Index)->entityRigidBody->getMotionState()->getWorldTransform(physicsTransform);

		float matrix[16];
		physicsTransform.getOpenGLMatrix(matrix);

		glm::mat4 modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::mat4(glm::make_mat4(matrix));

		modelMatrix = glm::scale(modelMatrix, ents.at(Index)->scale);

		//modelMatrix = glm::rotate(modelMatrix, glm::radians(ents.at(Index)->getRotation()), glm::vec3(1, 0, 0));//rotation x = 0.0 degrees
		//modelMatrix = glm::rotate(modelMatrix, 0.0f, glm::vec3(0, 1, 0));//rotation y = 0.0 degrees
	//	modelMatrix = glm::rotate(modelMatrix, 0.0f, glm::vec3(0, 0, 1));//rotation z = 0.0 degrees

		staticShader->setMat4("model", modelMatrix);
		return modelMatrix;
		
	}

	void setupNormalMatrix(glm::mat4 modelMatrix)
	{
		staticShader->use();
		glm::mat3 normalMatrix = glm::mat3(1.0f);
		normalMatrix = glm::mat3(glm::transpose(glm::inverse(modelMatrix)));

		staticShader->setMat3("normalMatrix", normalMatrix);
	}

	void init(Shader *entShader)
	{
		staticShader = entShader; //change this to init
	}

	Shader* getShader()
	{
		return staticShader;
	}


	void renderEntities(glm::mat4 proj, glm::mat4 viewM , Camera camera2)
	{
		
		setupShaderUniforms(proj, viewM , camera2);

		for (int i = 0; i < ents.size(); i++)
		{
			staticShader->use();
			if (glm::distance(ents.at(i)->getPosition(), camera2.getPosition())  > 100.0)
			{
				continue;
			}

			if (ents.at(i)->textComp == true)
			{
				glActiveTexture(GL_TEXTURE0);
				glGenerateMipmap(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, ents.at(i)->entTex.textId);
				glGenerateMipmap(GL_TEXTURE_2D);
			}

			if (ents.at(i)->pMaps == true)
			{
				staticShader->setBool("pMAP", true);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, ents.at(i)->diffuseTex.textId);

				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, ents.at(i)->specularTex.textId);
			
			}

			if (ents.at(i)->pbr == true)
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, ents.at(i)->albedoTex->textId);

				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, ents.at(i)->normalTex->textId);

				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, ents.at(i)->metallicTex->textId);

				glActiveTexture(GL_TEXTURE3);
				glBindTexture(GL_TEXTURE_2D, ents.at(i)->roughnessTex->textId);

				glActiveTexture(GL_TEXTURE4);
				glBindTexture(GL_TEXTURE_2D, ents.at(i)->aoTex->textId);

			}

			if (ents.at(i)->isModel == true)
			{
				staticShader->setFloat("extraRoughness", ents.at(i)->extraRough);
				staticShader->setFloat("extraMetal", ents.at(i)->extraMetal);
				

				if (ents.at(i)->modelEnt->isTransparent == true)
				{
					glDisable(GL_CULL_FACE);
				}
				glm::mat4 passMatrix = setupModelMatrix(i);
				setupNormalMatrix(passMatrix);
				ents.at(i)->modelEnt->Draw(staticShader);
			//	glEnable(GL_CULL_FACE);
			}

		

			else {

				
				glBindVertexArray(ents.at(i)->getVAO());
				glm::mat4 passMatrix= setupModelMatrix(i);
				setupNormalMatrix(passMatrix);  //THATS WHY NORMAL MATRX WASNT WORKING AS MODELS DOTN COME HERE!!
				glDrawArrays(GL_TRIANGLES, 0, 36);
				staticShader->setBool("pMAP", false);
			}
		}
	}


};