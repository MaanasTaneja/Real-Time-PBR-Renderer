#pragma once

class Skybox
{
public:
	Texture* cubeMap;
	unsigned int skyTexture;
	unsigned int hdrTexture;
	unsigned int irradianceMap;
	unsigned int prefilterMap;
	unsigned int brdfLUTTexture;
	unsigned int skyVAO, skyVBO;
	bool usesHDR = false;

	void attachHDRMap(string cubeFaces)
	{
		genVAO();
		unsigned int tmp = equiRectangulartoCubemap(loadHDRfile(cubeFaces));
		genirradianceMap(tmp);
		genprefiltermap(tmp);
		genbrdfLUT();
		hdrTexture = (tmp);
		usesHDR = true;
	}

	void attachCubeMap(vector<string> cubeFaces)
	{
		genVAO();
		unsigned int tmp = cubeMap->getCubeMapID(cubeFaces);
		genirradianceMap(tmp);
		genprefiltermap(tmp);
		genbrdfLUT();
		hdrTexture = tmp;
		usesHDR = true;
	}



	void genVAO()
	{
		glGenBuffers(1, &skyVBO);
		glBindBuffer(GL_ARRAY_BUFFER, skyVBO);
		glGenVertexArrays(1, &skyVAO);
		glBindVertexArray(skyVAO);

		glBufferData(GL_ARRAY_BUFFER, sizeof(cube), &cube, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
		glBindVertexArray(0);


	}

	void setCubeMap(unsigned int cube)
	{
		skyTexture = cube;
	}

	unsigned int genirradianceMap(unsigned int envMap)
	{
			unsigned int captureFBO;
			unsigned int captureRBO;
			glGenFramebuffers(1, &captureFBO);
			glGenRenderbuffers(1, &captureRBO);

			glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
			glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

			cout << "PRECOMPUTING::DIFFUSE::INTEGRAL" << endl;
			Shader irradianceShader("diffuseIBLVertex.vs", "diffuseIBLFrag.fs");
			
			glGenTextures(1, &irradianceMap);
			glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);

			for (unsigned int i = 0; i < 6; ++i)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
			}

			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			

			glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
			glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);


			irradianceShader.use();
			irradianceShader.setInt("environmentMap", 0);
			irradianceShader.setMat4("projection", captureProjection);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, envMap);

			glViewport(0, 0, 32, 32);
			glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
			for (unsigned int i = 0; i < 6; ++i)
			{
				irradianceShader.setMat4("view", captureViews[i]);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				renderCube();
			}
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			return irradianceMap;
	}

	unsigned int genprefiltermap(unsigned int envCubemap)
	{
		Shader prefilterShader("diffuseIBLVertex.vs", "specularIBLFrag.fs");
		unsigned int captureFBO;
		unsigned int captureRBO;
		glGenFramebuffers(1, &captureFBO);
		glGenRenderbuffers(1, &captureRBO);

		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);


		
		glGenTextures(1, &prefilterMap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // be sure to set minifcation filter to mip_linear 
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// generate mipmaps for the cubemap so OpenGL automatically allocates the required memory.
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		// pbr: run a quasi monte-carlo simulation on the environment lighting to create a prefilter (cube)map.
		// ----------------------------------------------------------------------------------------------------
		prefilterShader.use();
		prefilterShader.setInt("environmentMap", 0);
		prefilterShader.setMat4("projection", captureProjection);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		unsigned int maxMipLevels = 5;
		for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
		{
			// reisze framebuffer according to mip-level size.
			unsigned int mipWidth = 128 * std::pow(0.5, mip);
			unsigned int mipHeight = 128 * std::pow(0.5, mip);
			glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
			glViewport(0, 0, mipWidth, mipHeight);

			float roughness = (float)mip / (float)(maxMipLevels - 1);
			prefilterShader.setFloat("roughness", roughness);
			for (unsigned int i = 0; i < 6; ++i)
			{
				prefilterShader.setMat4("view", captureViews[i]);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);

				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				renderCube();
			}
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		return prefilterMap;
	}

	unsigned int genbrdfLUT()
	{
		unsigned int captureFBO;
		unsigned int captureRBO;
		glGenFramebuffers(1, &captureFBO);
		glGenRenderbuffers(1, &captureRBO);

		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

		Shader brdfShader("specularIBLbrdfVS.vs", "specularIBLbrdfFS.fs");

		glGenTextures(1, &brdfLUTTexture);

		// pre-allocate enough memory for the LUT texture.
		glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
		// be sure to set wrapping mode to GL_CLAMP_TO_EDGE
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// then re-configure capture framebuffer object and render screen-space quad with BRDF shader.
		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

		glViewport(0, 0, 512, 512);
		brdfShader.use();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		renderQuad();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return brdfLUTTexture;
	}

	

private:

	unsigned int loadHDRfile(string hdr)
	{
		stbi_set_flip_vertically_on_load(true);
		int width, height, nrComponents;
		float* data = stbi_loadf(hdr.c_str(), &width, &height, &nrComponents, 0);
		unsigned int hdrTexture = 0;
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
		stbi_set_flip_vertically_on_load(false);
		return hdrTexture;
	}



	  float cube[120] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	  glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	  glm::mat4 captureViews[50] =
	  {
		  glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		  glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		  glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		  glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		  glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		  glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	  };


	  unsigned int equiRectangulartoCubemap(unsigned int hdrTexture)
	  {
		  unsigned int captureFBO;
		  unsigned int captureRBO;
		  glGenFramebuffers(1, &captureFBO);
		  glGenRenderbuffers(1, &captureRBO);

		  glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		  glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
		  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);


		  Shader equirectangularToCubemapShader("hdrToCubeVS.vs", "hdrToCubeFS.fs");
		  unsigned int envCubemap;
		  glGenTextures(1, &envCubemap);
		  glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
		  for (unsigned int i = 0; i < 6; ++i)
		  {
			  glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
		  }
		  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		  equirectangularToCubemapShader.use();
		  equirectangularToCubemapShader.setInt("equirectangularMap", 0);
		  equirectangularToCubemapShader.setMat4("projection", captureProjection);
		  glActiveTexture(GL_TEXTURE0);
		  glBindTexture(GL_TEXTURE_2D, hdrTexture);

		  glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
		  glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		  for (unsigned int i = 0; i < 6; ++i)
		  {
			  equirectangularToCubemapShader.setMat4("view", captureViews[i]);
			  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
			  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			  renderCube();
		  }
		  glBindFramebuffer(GL_FRAMEBUFFER, 0);

		  return envCubemap;
	  }

	  void renderCube()
	  {
		  unsigned int cubeVAO = 0;
		  unsigned int cubeVBO = 0;

		  float vertices[] = {
			  // back face
			  -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			   1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			   1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
			   1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			  -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			  -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
			  // front face
			  -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			   1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
			   1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			   1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			  -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
			  -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			  // left face
			  -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			  -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
			  -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			  -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			  -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			  -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			  // right face
			   1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			   1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			   1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
			   1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			   1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			   1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
			  // bottom face
			  -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			   1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
			   1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			   1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			  -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			  -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			  // top face
			  -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			   1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			   1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
			   1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			  -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			  -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
		  };
		  glGenVertexArrays(1, &cubeVAO);
		  glGenBuffers(1, &cubeVBO);
		  // fill buffer
		  glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		  // link vertex attributes
		  glBindVertexArray(cubeVAO);
		  glEnableVertexAttribArray(0);
		  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		  glEnableVertexAttribArray(1);
		  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		  glEnableVertexAttribArray(2);
		  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		  glBindBuffer(GL_ARRAY_BUFFER, 0);
		  glBindVertexArray(0);

		  // render Cube
		  glBindVertexArray(cubeVAO);
		  glDrawArrays(GL_TRIANGLES, 0, 36);
		  glBindVertexArray(0);
	  }

	  void renderQuad()
	  {
		  unsigned int quadVAO = 0;
		  unsigned int quadVBO;
	
			  float quadVertices[] = {
				  // positions        // texture Coords
				  -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
				  -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
				   1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
				   1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			  };
			  // setup plane VAO
			  glGenVertexArrays(1, &quadVAO);
			  glGenBuffers(1, &quadVBO);
			  glBindVertexArray(quadVAO);
			  glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
			  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
			  glEnableVertexAttribArray(0);
			  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			  glEnableVertexAttribArray(1);
			  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		  
		  glBindVertexArray(quadVAO);
		  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		  glBindVertexArray(0);
	  }
};