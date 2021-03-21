#pragma once

glm::mat4 generateModelMatrix(Model model, glm::vec3 scale)
{
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::scale(modelMatrix, scale);
	return modelMatrix;
}

