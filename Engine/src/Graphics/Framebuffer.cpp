#include "Framebuffer.h"

Framebuffer::Framebuffer(std::vector<Buffer*> buffers)
{
	glGenFramebuffers(1, &mFbID);

	mBufferCount = buffers.size();
	mBuffers.resize(static_cast<size_t>(mBufferCount));
	mTexID.resize(static_cast<size_t>(mBufferCount));
	mSize.resize(static_cast<size_t>(mBufferCount));

	BindFramebuffer();

	for (unsigned i = 0; i < mBufferCount; i++)
	{
		Buffer* buf = buffers[i];
		mBuffers[i] = buf->mBufferType;
		mSize[i] = buf->mSize;
		mTexID[i] = buf->Construct(i);
	}

	UnbindFramebuffer();
}

void Framebuffer::Initialize(const bool useDepthRBO)
{
	BindFramebuffer();

	unsigned int rboVal;
	if (useDepthRBO)
	{
		glGenRenderbuffers(1, &rboVal);
		glBindRenderbuffer(GL_RENDERBUFFER, rboVal);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, BufferSize().x, BufferSize().y);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboVal);
	}

	std::vector<GLenum> drawBuffers;
	for (unsigned i = 0; i < mBuffers.size(); i++)
	{
		switch (mBuffers[i])
		{
		case Buffer::BufferType::DepthBuffer:
			drawBuffers.push_back(GL_NONE);
			break;
		default:
			drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + i);
			break;
		}
	}
	glDrawBuffers(static_cast<GLsizei>(mBuffers.size()), &drawBuffers[0]);

	//assertThis(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE, "Framebuffer is not complete.");

	UnbindFramebuffer();
}

void Framebuffer::BindTexture(const unsigned idx)
{
	glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(mTexID[idx]));
}

void Framebuffer::UnbindTexture()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Framebuffer::BindFramebuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, mFbID);
}

void Framebuffer::UnbindFramebuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


GLuint ColorBuffer::Construct(const size_t bufferIdx) const
{
	GLuint texID;
	glGenTextures(1, &texID);

	glBindTexture(GL_TEXTURE_2D, texID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mSize.x, mSize.y, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

	// Filtering 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Expansion
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + static_cast<GLenum>(bufferIdx), GL_TEXTURE_2D, texID, 0);

	return texID;
}

GLuint HDRBuffer::Construct(const size_t bufferIdx) const
{
	GLuint texID;
	glGenTextures(1, &texID);

	glBindTexture(GL_TEXTURE_2D, texID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, mSize.x, mSize.y, 0, GL_RGBA, GL_FLOAT, nullptr);

	// Filtering 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Expansion
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + static_cast<GLenum>(bufferIdx), GL_TEXTURE_2D, texID, 0);

	return texID;
}

GLuint DepthBuffer::Construct(const size_t bufferIdx) const
{
	GLuint texID;
	glGenTextures(1, &texID);

	glBindTexture(GL_TEXTURE_2D, texID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, mSize.x, mSize.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

	// Filtering 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Expansion
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texID, 0);

	return texID;
}

GLuint StencilBuffer::Construct(const size_t bufferIdx) const
{
	// TODO: Stencil buffer
	GLuint texID = 0;
	return texID;
}