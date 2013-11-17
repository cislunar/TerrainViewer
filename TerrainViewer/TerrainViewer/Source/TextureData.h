#pragma once
#include "Render.h"
#include "Math.h"
#include <stdint.h>

class TextureData
{
public:
	TextureData()
	{
		m_texData = NULL;
	}
	~TextureData()
	{
		if(m_texData != NULL)
		{
			free(m_texData);
			m_texData = NULL;
		}
	}

	void SetupData( size_t _bytes)
	{
		if(m_texData != NULL)
		{
			free(m_texData);
		}
		m_texData = (unsigned char*)malloc( _bytes );
	}

	int TexByteWidth()
	{
		return m_texWidth * m_channels;
	}

	GLenum m_texType, m_format;
	GLint m_texWidth, m_texHeight;
	uint32_t m_byteCnt;
	uint8_t m_channels;
	unsigned char* m_texData;

};