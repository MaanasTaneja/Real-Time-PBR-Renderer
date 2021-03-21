#pragma once

class PostProcesser
{
public:

	Shader* screenShader;
	unsigned int quadVBO, quadVAO;
	FrameBuffer post;
	unsigned int multisampledFBO, multisampledRBO;
	unsigned int intermediateFBO, intermediateRBO;
	unsigned int multisampleColorTexture;

	unsigned int screenTexture[2];

	unsigned int framebuffer;
	unsigned int rbo;
	unsigned int textureColorBufferMultiSampled;

	unsigned int bloomFBO, bloomRBO;
	unsigned int brightFilterFBO, brightFilterRBO;
	unsigned int hBlurFBO, vBlurFBO;


	void attachPostshader(string vPath, string fPath)
	{
		screenShader = new Shader(vPath, fPath);
		screenShader->use();
		screenShader->setInt("screenTexture", 0);
	}

	void genScreenQuad()
	{

		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));


	}

	void begin()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	}

	void end()
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediateFBO);
		glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void drawPostQuad()
	{
		screenShader->use();
		screenShader->setFloat("time", glfwGetTime());

		glBindVertexArray(quadVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, screenTexture[0]);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	void initPostProcesser()
	{
		prepareFrameBuffers();
		genScreenQuad();
	}


private:

	void prepareFrameBuffers()
	{

		glGenFramebuffers(1, &framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		// create a multisampled color attachment texture

		glGenTextures(1, &textureColorBufferMultiSampled);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, MSAA_SAMPLE_LEVEL , GL_RGB, SCR_WIDTH, SCR_HEIGHT, GL_TRUE);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled, 0);
		// create a (also multisampled) renderbuffer object for depth and stencil attachments

		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, MSAA_SAMPLE_LEVEL, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// configure second post-processing framebuffer

		glGenFramebuffers(1, &intermediateFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO);
		// create a color attachment texture

		glGenTextures(2, screenTexture);

		for (int i = 0; i < 2; i++)
		{
			glBindTexture(GL_TEXTURE_2D, screenTexture[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, screenTexture[i], 0);	// we only need a color buffer
		}

		unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(2, attachments);
	


		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			cout << "ERROR::FRAMEBUFFER:: Intermediate framebuffer is not complete!" << endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	
	
	float quadVertices[25] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	 // positions   // texCoords
	 -1.0f,  1.0f,  0.0f, 1.0f,
	 -1.0f, -1.0f,  0.0f, 0.0f,
	  1.0f, -1.0f,  1.0f, 0.0f,

	 -1.0f,  1.0f,  0.0f, 1.0f,
	  1.0f, -1.0f,  1.0f, 0.0f,
	  1.0f,  1.0f,  1.0f, 1.0f
	};
};
