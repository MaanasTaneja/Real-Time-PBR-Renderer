#pragma once

class AreaLight
{
public:
	string type;
	float sphere_radius;
	glm::vec3 position;

	float rect_width;
	float rect_height;

	glm::vec3 rect_up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 rect_front = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 rect_right = glm::vec3(1.0f, 0.0f, 0.0f);

	glm::mat4 lightMatrix = glm::mat4(1.0f);

	AreaLight(float radius, glm::vec3 position_)
	{
		sphere_radius = radius;
		position = position_;
		type = "sphere";

		lightMatrix = glm::translate(lightMatrix, position_);
		lightMatrix = glm::scale(lightMatrix, glm::vec3(radius, radius, radius));
	}

	AreaLight(float width, float height, float rotation, glm::vec3 position_)
	{
		rect_width = width;
		rect_height = height;
		position = position_;

	
		lightMatrix = glm::translate(lightMatrix, position_);
		lightMatrix = glm::rotate(lightMatrix, glm::radians(0.0f), glm::vec3(1, 0, 0));//rotation x = 0.0 degrees
		lightMatrix = glm::rotate(lightMatrix, glm::radians(rotation), glm::vec3(0, 1, 0));//rotation y = 0.0 degrees
		lightMatrix = glm::rotate(lightMatrix, glm::radians(0.0f), glm::vec3(0, 0, 1));//rotation z = 0.0 degrees

		rect_up *= glm::mat3(lightMatrix);
		rect_front *= glm::mat3(lightMatrix);
		rect_right *= glm::mat3(lightMatrix);
		type = "rectangle";
	}
};