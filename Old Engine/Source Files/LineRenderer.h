#pragma once
#include <vector>

Shader* lineShader_;


class LineRenderer
{

public:

	glm::vec3 start;
	glm::vec3 end;

	glm::mat4 proj;
	glm::mat4 view;

	float lineThickness;
	glm::vec3 lineColor;

	void setPositions(glm::vec3 start_, glm::vec3 end_)
	{
		start = start_;
		end = end_;
	}

	void setupShaderUniforms(glm::mat4 projection_, glm::mat4 view_) //temp to be shifted to masterrednerer
	{

		lineShader_->use();
		lineShader_->setMat4("projection", glm::mat4(projection_));
		lineShader_->setMat4("view", glm::mat4(view_));
		lineShader_->setVec3("lineColor", lineColor);

	}

	glm::mat4 setupModelMatrix()  //SETS UP ONLY MODEL BOI
	{
		lineShader_->use();
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f));

		lineShader_->setMat4("model", modelMatrix);
		return modelMatrix;

	}

	void updateProjections(glm::mat4 pr, glm::mat4 view_)
	{
		proj = pr;
		view = view_;
	
	}

	void init()
	{
		lineShader_ = new Shader("lineShader.vs", "lineShader.fs");
	}

	void renderLines()
	{

		setupShaderUniforms(proj, view);

		glBindVertexArray(generateVAO());
		setupModelMatrix();
		glLineWidth(lineThickness);
		glDrawArrays(GL_LINES, 0, 2);
		
		
	}

	unsigned int generateVAO()
	{
		float vertices[6] = {
			start.x , start.y , start.z,
			end.x , end.y , end.z
		};

		unsigned int VBO, VAO;
		glGenBuffers(1, &VBO);
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
		return VAO;
	}


};