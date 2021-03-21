#pragma once

class FrameBuffer
{
public:
	unsigned int colorAttach;
	unsigned int ScreenShader;

	unsigned int FBO;
	unsigned int RBO;

	int fHeight = 0;
	int fWidth =  0;

	void bindFrameBuffer()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		glViewport(0, 0, fWidth, fHeight);
	}

	void resolveFrameBuffer()
	{
		glBindFramebuffer(GL_READ_BUFFER, FBO);
		glBindFramebuffer(GL_DRAW_BUFFER, 0);
		glViewport(0, 0, fWidth, fHeight);
		glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}

	void unbindFramebuffer()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	}


	void gen_FrameBuffer()
	{
		glGenFramebuffers(1, &FBO);
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	}

	void gen_RenderBuffer()
	{
		glGenRenderbuffers(1, &RBO);
		glBindRenderbuffer(GL_RENDERBUFFER, RBO);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER,4, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
	}

	void addTextureAttachment()
	{
		fWidth = SCR_WIDTH;
		fHeight = SCR_HEIGHT;
		glBindBuffer(GL_FRAMEBUFFER, FBO);
		glGenTextures(1, &colorAttach);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, colorAttach);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, SCR_WIDTH, SCR_HEIGHT, GL_TRUE);
		glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, colorAttach, 0);
	}

	void addTextureAttachment(int width , int height)
	{
		fWidth = width;
		fHeight = height;

		glBindBuffer(GL_FRAMEBUFFER, FBO);
		glGenTextures(1, &colorAttach);
		glBindTexture(GL_TEXTURE_2D, colorAttach);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAttach, 0);
	}

	void checkerrors()
	{
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
		else
		{
			cout << "FRAMEBUFFER::COMPLETE!" << endl;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}



};