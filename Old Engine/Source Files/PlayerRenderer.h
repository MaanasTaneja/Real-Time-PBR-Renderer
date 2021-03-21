#pragma once

Shader* playerShader;

class PlayerRenderer
{
public:

	Player* player;

	void init(Shader* playShader)
	{
		playerShader = playShader;
	}

	void addPlayer(Player* play)
	{
		player = play;
	}


	void setupShaderUniforms(glm::mat4 projection_, glm::mat4 view_, Camera camera3) //temp to be shifted to masterrednerer
	{

		playerShader->use();
		playerShader->setMat4("projection", glm::mat4(projection_));
		playerShader->setMat4("view", glm::mat4(view_));
		playerShader->setVec3("skyColor", glm::vec3(1.0f, 1.0f, 1.0f));

	}


	Shader* getShader()
	{
		return playerShader;
	}

	void renderPlayer(glm::mat4 proj, glm::mat4 viewM, Camera camera2)
	{
		playerShader->use();
	
		setupShaderUniforms(proj, viewM, camera2);
		player->update();
	}

		
};
