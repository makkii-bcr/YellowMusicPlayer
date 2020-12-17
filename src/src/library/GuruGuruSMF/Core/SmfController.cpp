
#include "stdafx.h"

#include "SmfController.h"

namespace GuruGuruSmf { namespace Core {

	// �R���X�g���N�^
	SmfController::SmfController()
	{
		smf = new Smf();
	}

	// �f�X�g���N�^
	SmfController::~SmfController()
	{
		delete smf;
	}
	
	// ������
	void SmfController::Initialize(Analyzer::Analyzer* analyzerObject, Compiler::Compiler* compilerObject)
	{
		analyzer = analyzerObject;
		compiler = compilerObject;
	}

	// SMF���g�p�\��Ԃɂ���
	GGSERROR SmfController::Open()
	{
		GGSERROR err = smf->Reload();
		if (err != GgsError::NoError) return err;
		compiler->Compile(smf);
		smf->ReleaseSmfBinary();
		return GgsError::NoError;
	}

	// �t�@�C������ǂݍ���
	GGSERROR SmfController::LoadFromFile(LPCWSTR fileName)
	{
		GGSERROR err = smf->LoadFromFile(fileName);
		if (err != GgsError::NoError)	return err;
		
		err = analyzer->Analyze(smf);
		if (err != GgsError::NoError)	return err;
		
		OnAfterAnalyzed();
		
		smf->ReleaseSmfBinary();
		
		return GgsError::NoError;
	}

	// ����������ǂݍ���
	GGSERROR SmfController::LoadFromMemory(const byte* smfBinary, int length)
	{
		GGSERROR err = smf->LoadFromMemory(smfBinary, length);
		if (err != GgsError::NoError)	return err;
		
		err = analyzer->Analyze(smf);
		if (err != GgsError::NoError)	return err;
		
		OnAfterAnalyzed();
		
		smf->ReleaseSmfBinary();
		
		return GgsError::NoError;
	}


	//
	// SmfController3Buffered
	//

	// �R���X�g���N�^
	SmfController3Buffered::SmfController3Buffered()
	{
		smfBuffer = 0;
	}

	// �f�X�g���N�^
	SmfController3Buffered::~SmfController3Buffered()
	{
		delete[] smfBuffer;
	}

	// SMF���g�p�\��Ԃɂ���
	GGSERROR SmfController3Buffered::Open()
	{
		GGSERROR err = GetSmf()->Reload();
		if (err != GgsError::NoError) return err;
		GetCompiler()->Compile(GetSmf());
		return GgsError::NoError;
	}

	// ����������ǂݍ���(���̎������������ɕۑ�)
	GGSERROR SmfController3Buffered::LoadFromMemory(const byte* smfBinary, int length)
	{
		smfBuffer = new byte[length];
		memcpy(smfBuffer, smfBinary, length);
		return SmfController::LoadFromMemory(smfBuffer, length);
	}

}}





