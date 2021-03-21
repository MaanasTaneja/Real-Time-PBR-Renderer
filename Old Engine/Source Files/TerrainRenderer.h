#pragma once

Shader* terrainShader;

class TerrainRenderer
{
public:
	
	vector<Terrain> terrains;

	void addTerrain(Terrain terrain)
	{
		terrains.push_back(terrain);
		cout << "TERRAIN_RENDERER::TERRAIN [VAO] " << terrain.terrainVAO << " to render at" << " X  " << terrain.getX() << " Y , " << terrain.getZ() << endl;
	}

	void setupShaderUniforms(glm::mat4 projection_, glm::mat4 view_) //temp to be shifted to masterrednerer
	{

		terrainShader->use();
		terrainShader->setMat4("projection", glm::mat4(projection_));
		terrainShader->setMat4("view", glm::mat4(view_));
		terrainShader->setVec3("skyColor", glm::vec3(1.0f, 1.0f, 1.0f));

	}

	void setupModelMatrix(int Index)  //SETS UP ONLY MODEL BOI
	{
		terrainShader->use();
		glm::vec3 terrPos = glm::vec3(terrains.at(Index).getX(), 0, terrains.at(Index).getZ());
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, terrPos);
		terrainShader->setMat4("model", model);
	}

	void init(Shader* terrShader)
	{
		terrainShader = terrShader; //change this to init
	}

	Shader* getShader()
	{
		return terrainShader;
	}

	void renderTerrains(glm::mat4 proj, glm::mat4 viewM, Camera camera2)
	{

		setupShaderUniforms(proj, viewM);

		for (int i = 0; i < terrains.size(); i++)
		{
			terrainShader->use();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, terrains.at(i).terrainTexture);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, terrains.at(i).normalMap);

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, terrains.at(i).metallicMap);

			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, terrains.at(i).roughnessMap);

			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, terrains.at(i).aoMap);

			glActiveTexture(GL_TEXTURE5);
			glBindTexture(GL_TEXTURE_2D, terrains.at(i).blend);

			glActiveTexture(GL_TEXTURE6);
			glBindTexture(GL_TEXTURE_2D, terrains.at(i).rBlend);

			glActiveTexture(GL_TEXTURE7);
			glBindTexture(GL_TEXTURE_2D, terrains.at(i).gBlend);

			glActiveTexture(GL_TEXTURE8);
			glBindTexture(GL_TEXTURE_2D, terrains.at(i).bBlend);

			glActiveTexture(GL_TEXTURE9);
			glBindTexture(GL_TEXTURE_2D, terrains.at(i).defaultN);

			glBindVertexArray(terrains.at(i).terrainVAO);

			setupModelMatrix(i);
			glDrawElements(GL_TRIANGLES,97536 , GL_UNSIGNED_INT, 0);   //render

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, 0);



			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, 0);

			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, 0);

			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, 0);

			glActiveTexture(GL_TEXTURE5);
			glBindTexture(GL_TEXTURE_2D, 0);

			glActiveTexture(GL_TEXTURE6);
			glBindTexture(GL_TEXTURE_2D, 0);

			glActiveTexture(GL_TEXTURE7);
			glBindTexture(GL_TEXTURE_2D, 0);

			glActiveTexture(GL_TEXTURE8);
			glBindTexture(GL_TEXTURE_2D,0);

			glActiveTexture(GL_TEXTURE9);
			glBindTexture(GL_TEXTURE_2D,0);
			
			//glDrawArrays(GL_TRIANGLES, 0, 128*128);
		}
	}

};