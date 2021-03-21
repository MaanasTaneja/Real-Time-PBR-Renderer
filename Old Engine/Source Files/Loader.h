#pragma once

class Loader
{
public:

	std::vector<GLuint> m_vaos;
	std::vector<GLuint> m_vbos;
	std::vector<GLuint> m_textures;

	  Loader()
	{
		// Initialize the storage arrays
		m_vaos.clear();
		m_vbos.clear();
		m_textures.clear();
	}



	unsigned int LoadToVAO(std::vector<glm::vec3> vertices, std::vector<glm::vec2> textures, std::vector<glm::vec3> normals, std::vector<int> indices)
	{
		// create a new VAO
		GLuint vaoID = CreateVAO();
		int indicesSize = indices.size();
		BindIndicesBuffer(indices.data(), indicesSize);
		// Store the data in attribute lists
		StoreDataInAttributeList(0, 3, &vertices[0], vertices.size() * sizeof(glm::vec3));
		glEnableVertexAttribArray(0);
		StoreDataInAttributeList(1, 2, &textures[0], textures.size() * sizeof(glm::vec2));
		glEnableVertexAttribArray(1);
		StoreDataInAttributeList(2, 3, &normals[0], normals.size() * sizeof(glm::vec3));
		glEnableVertexAttribArray(2);
	
		return vaoID;
	}




	GLuint CreateVAO()
	{
		GLuint vaoID;
		// Create a new VAO
		glGenVertexArrays(1, &vaoID);
		// Store the vao in the list
		m_vaos.push_back(vaoID);
		// Bind the VAO to use it
		glBindVertexArray(vaoID);
		return vaoID;
	}


	void StoreDataInAttributeList(GLuint attribNumber, int attribSize, void* data, int dataSize)
	{
		GLuint vboID;
		// Create a new buffer
		glGenBuffers(1, &vboID);
		// Store the buffer in the list
		m_vbos.push_back(vboID);
		// Bind the buffer to use it
		glBindBuffer(GL_ARRAY_BUFFER, vboID);
		// Store the data in the buffer
		glBufferData(GL_ARRAY_BUFFER, dataSize, data, GL_STATIC_DRAW);
		// Tell OpenGL how and where to store this VBO in the VAO
		glVertexAttribPointer(attribNumber, attribSize, GL_FLOAT, GL_FALSE, 0, nullptr);
	}


	void BindIndicesBuffer(int* indices, int& count)
	{
		GLuint vboID;
		// Generate a buffer and bind it for use
		glGenBuffers(1, &vboID);
		// Store the buffer in the list
		m_vbos.push_back(vboID);
		// Bind the buffer to use it
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboID);
		// Store the indices in the buffer
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * count, indices, GL_STATIC_DRAW);
	}
};