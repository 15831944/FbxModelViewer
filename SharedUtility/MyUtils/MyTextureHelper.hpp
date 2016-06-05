#pragma once

#include "MyMath.hpp"


namespace MyTextureHelper
{
	class TextureDataPack final
	{
		// DIB �s�N�Z���̕��т��K��E�ۏ؂�������΁AWICPixelFormatGUID ���g���Ηǂ��̂����c�c�� MS �v���b�g�t�H�[���ւ̈ڐA�������ɂȂ�B
		// �Ǝ��� enum ��݂���Ȃ肷�ׂ��B���Ȃ݂Ɍ���ł͈��k�o�b�t�@�ɂ͔�Ή��B
		// �Ȃ��ADirect3D 10 �ȍ~�̓p���b�g �e�N�X�`���i�C���f�b�N�X�摜�j�͑��݂��Ȃ��̂ŁA
		// �O���[�X�P�[���ȊO�� 256 �F�摜�Ȃǂ��ǂ����Ă��K�v�ł���� Texture1D �Ȃǂ��p���b�g����ɂ��ăV�F�[�_�[�őΉ�����B
	public:
		uint32_t TextureWidth;
		uint32_t TextureHeight;
		uint32_t TextureColorDepthInBits;
		bool IsForAlpha;
		std::vector<uint8_t> TextureDib;
	public:
		TextureDataPack()
			: TextureWidth()
			, TextureHeight()
			, TextureColorDepthInBits()
			, IsForAlpha()
		{}

	public:
		uint32_t GetStrideInBytes() const
		{
			return MyMath::CalcStrideInBytes(this->TextureWidth, this->TextureColorDepthInBits);
		}
	};

	typedef std::shared_ptr<TextureDataPack> TTextureDataPackPtr;


	class FontTextureDataPack final
	{
	public:
		TextureDataPack TextureData;
		MyMath::TCharCodeUVMap CodeUVMap;
		long FontHeight;
		bool UsesMonospaceFont;
	public:
		FontTextureDataPack()
			: FontHeight()
			, UsesMonospaceFont()
		{}
	};

	typedef std::shared_ptr<FontTextureDataPack> TFontTextureDataPackPtr;

	// �e�N�X�`�� OFF �ݒ�p�ȂǂɎg���_�~�[ �z���C�g �e�N�X�`���̃T�C�Y�B�Ƃ肠����2�ׂ̂���T�C�Y�ɂ��Ă����B1x1 �ł�������͗l�B
#if 0
	const UINT DUMMY_WHITE_TEX_SIZE = 4;
#elif 0
	const UINT DUMMY_WHITE_TEX_SIZE = 2;
#else
	const UINT DUMMY_WHITE_TEX_SIZE = 1;
#endif

	// �e�N�X�`�� �T�C�Y���ő�K�����i�O���f�[�V���� �X�g�b�v���j�ɂȂ�B256 �K������Βʏ�� LDR �Ɠ����V�F�[�f�B���O���\�B
	// 128 �K�����Ƃ��Ă��T���v�����O����Ƃ��Ƀo�C���j�A��ԂȂǂ̃T���v���[���g���΁A
	// ���j�A�ȃO���f�[�V�����Ɋւ��Ă͑卷�͂Ȃ��B
#if 0
	const UINT TOON_SHADING_REF_TEX_SIZE = 128;
#else
	const UINT TOON_SHADING_REF_TEX_SIZE = 256;
#endif

	inline float CalcBilinearToonShadingGradientRefTexCoordV(int toonMaterialIndex)
	{
		return (toonMaterialIndex + 0.5f) / MyTextureHelper::TOON_SHADING_REF_TEX_SIZE;
	}

} // end of namespace
