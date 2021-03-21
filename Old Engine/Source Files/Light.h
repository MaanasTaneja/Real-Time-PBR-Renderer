#pragma once



class Light
{
public:

	Model* lampModel;
	unsigned int lVAO;
	glm::vec3 pos;
	glm::vec3 col;
	Shape debug;
	

	void addDebugLamp()
	{
		lVAO = debug.cube();
		
	}

	glm::vec3 getPosition()
	{
		return pos;
	}

	unsigned int getVAO()
	{
		return lVAO;
	}

	void setupShaderUniforms(glm::mat4 projection_, glm::mat4 view_, Camera camera3 , Shader* lampShader) //temp to be shifted to masterrednerer
	{

		lampShader->use();
		lampShader->setMat4("projection", glm::mat4(projection_));
		lampShader->setMat4("view", glm::mat4(view_));
		lampShader->setVec3("skyColor", glm::vec3(1.0f, 1.0f, 1.0f));

	}

	void setupModelMatrix(Shader* lampShader)  //SETS UP ONLY MODEL BOI
	{
		lampShader->use();
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(getPosition()));
		model = glm::scale(model, glm::vec3(0.5f));
		//model = glm::rotate(model, glm::vec3(ents.at(Index)->getRotation()));
		lampShader->setMat4("model", model);
	}


	void renderLights(glm::mat4 proj, glm::mat4 viewM, Camera camera2, Shader* lampShader_)
	{

		setupShaderUniforms(proj, viewM, camera2,  lampShader_);

    	lampShader_->use();

			if (glm::distance(getPosition(), camera2.getPosition()) < 100.0)
			{
				glBindVertexArray(getVAO());
				setupModelMatrix(lampShader_);
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
			

		}
	

};