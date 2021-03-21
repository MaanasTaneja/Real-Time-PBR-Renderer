#pragma once
#include <vector>

using namespace std;
class Texture
{
public:

	unsigned textId;


	unsigned int getTexID(string path)
	{
		try
		{
			glGenTextures(1, &textId);

			int width, height, nrComponents;
			unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
			if (data)
			{
				GLenum format;
				if (nrComponents == 1)
					format = GL_RED;
				else if (nrComponents == 3)
					format = GL_RGB;
				else if (nrComponents == 4)
					format = GL_RGBA;
			
				glBindTexture(GL_TEXTURE_2D, textId);
				glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
				glGenerateMipmap(GL_TEXTURE_2D);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				
				stbi_image_free(data);
			}
			else
			{
				std::cout << "Texture failed to load at path: " << path.c_str() <<endl;
				stbi_image_free(data);
			}
		}
		catch (exception& e)
		{
			cout << path << "SALE KY APROBLEM HAI" << endl;
		}

		return textId;
	}

	unsigned int getCubeMapID(vector<string> faces)
	{
		unsigned int cubeMap;
		glGenTextures(1, &cubeMap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

		int width, height, nrChannels;
		for (int i = 0; i < 6; i++)
		{
			unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
				);
				stbi_image_free(data);
			}

			else
			{
				std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
				stbi_image_free(data);
			}
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		return cubeMap;
	}

	unsigned int loadHDRfile(string hdr)
	{
		stbi_set_flip_vertically_on_load(true);
		int width, height, nrComponents;
		float* data = stbi_loadf(hdr.c_str(), &width, &height, &nrComponents, 0);
		unsigned int hdrTexture=0;
		if (data)
		{
			glGenTextures(1, &hdrTexture);
			glBindTexture(GL_TEXTURE_2D, hdrTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data); // note how we specify the texture's data value to be float

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		}
		else
		{
			std::cout << "Failed to load HDR image." << std::endl;
		}

		return hdrTexture;
	}


};
