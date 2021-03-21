#pragma once
Shader* lightShader;
Shader* mainShader;

class AreaLightsRenderer
{
public:
	vector<AreaLight> areaLights;
	int nAreas = 0;

	void addAreaLight(AreaLight light)
	{
		areaLights.push_back(light);
		nAreas++;
	}

	void init(Shader* mainShader_)
	{
		lightShader = new Shader("lightVertex.vs", "lightFragment.fs");
		mainShader = mainShader_;
	}


	void setupShaderUniforms(glm::mat4 projection_, glm::mat4 view_, Camera camera3) //temp to be shifted to masterrednerer
	{
		
		lightShader->use();
		lightShader->setMat4("projection", glm::mat4(projection_));
		lightShader->setMat4("view", glm::mat4(view_));
		lightShader->setVec3("skyColor", glm::vec3(1.0f, 1.0f, 1.0f));

	}

	void loadLightPositions()
	{
		mainShader->setInt("nAreaLights", nAreas);
		for (int i = 0; i < areaLights.size(); i++)
		{
			mainShader->setVec3("areaLightPositions[" + std::to_string(i) + "]", areaLights.at(i).position);
		}

	}

	void setupModelMatrix(int index)
	{
		mainShader->use();
		mainShader->setVec3("areaRect_up", areaLights.at(index).rect_up);
		mainShader->setVec3("areaRect_left", -areaLights.at(index).rect_right);
		mainShader->setFloat("areaSphere_radius", -areaLights.at(index).sphere_radius);
		
		
		lightShader->use();
		lightShader->setMat4("model", areaLights.at(index).lightMatrix);
	}

	void renderAreaLights(glm::mat4 projection, glm::mat4 view, Camera cam)
	{
		setupShaderUniforms(projection, view, cam);
		loadLightPositions();

		for (int i = 0; i < areaLights.size(); i++)
		{
			setupModelMatrix(i);
			if (areaLights.at(i).type.compare("sphere") == 0)
			{
				renderSphere();
			}

			else if (areaLights.at(i).type.compare("rectangle") == 0)
			{
				renderRectangle(areaLights.at(i).rect_width, areaLights.at(i).rect_height);
			}
		}
	}

private:

	unsigned int rectangleVAO = 0;
	void renderRectangle(float length, float width)
	{
		float vertices[30] = {};


		unsigned int VBO;
		glGenBuffers(1, &VBO);
		glGenVertexArrays(1, &rectangleVAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBindVertexArray(rectangleVAO);

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
		

		glDrawArrays(GL_ARRAY_BUFFER, 0, 6);
	}

	unsigned int sphereVAO = 0;
	unsigned int indexCount;
	void renderSphere()
	{
		if (sphereVAO == 0)
		{
			glGenVertexArrays(1, &sphereVAO);

			unsigned int vbo, ebo;
			glGenBuffers(1, &vbo);
			glGenBuffers(1, &ebo);

			std::vector<glm::vec3> positions;
			std::vector<glm::vec2> uv;
			std::vector<glm::vec3> normals;
			std::vector<unsigned int> indices;

			const unsigned int X_SEGMENTS = 64;
			const unsigned int Y_SEGMENTS = 64;
			
			for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
			{
				for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
				{
					float xSegment = (float)x / (float)X_SEGMENTS;
					float ySegment = (float)y / (float)Y_SEGMENTS;
					float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
					float yPos = std::cos(ySegment * PI);
					float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

					positions.push_back(glm::vec3(xPos, yPos, zPos));
					uv.push_back(glm::vec2(xSegment, ySegment));
					normals.push_back(glm::vec3(xPos, yPos, zPos));
				}
			}

			bool oddRow = false;
			for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
			{
				if (!oddRow) // even rows: y == 0, y == 2; and so on
				{
					for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
					{
						indices.push_back(y * (X_SEGMENTS + 1) + x);
						indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
					}
				}
				else
				{
					for (unsigned int x = X_SEGMENTS; x >= 0; --x)
					{
						indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
						indices.push_back(y * (X_SEGMENTS + 1) + x);
					}
				}
				oddRow = !oddRow;
			}
			indexCount = indices.size();

			std::vector<float> data;
			for (std::size_t i = 0; i < positions.size(); ++i)
			{
				data.push_back(positions[i].x);
				data.push_back(positions[i].y);
				data.push_back(positions[i].z);
				if (uv.size() > 0)
				{
					data.push_back(uv[i].x);
					data.push_back(uv[i].y);
				}
				if (normals.size() > 0)
				{
					data.push_back(normals[i].x);
					data.push_back(normals[i].y);
					data.push_back(normals[i].z);
				}
			}
			glBindVertexArray(sphereVAO);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
			float stride = (3 + 2 + 3) * sizeof(float);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(float)));
		}

		glBindVertexArray(sphereVAO);
		glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
	}
};
