// SMF����N���X

// #pragma once
#ifndef _SMFCONTROLLER_H_
#define _SMFCONTROLLER_H_

#include "Interface.h"
#include "Analyzer.h"
#include "Compiler.h"

namespace GuruGuruSmf { namespace Core {

	/// <summary>
	/// SMF����N���X
	/// </summary>
	class SmfController : public ISmfController
	{
	private:
		Smf* smf;
		Analyzer::Analyzer* analyzer;
		Compiler::Compiler* compiler;
		
	public:
		SmfController();			// �R���X�g���N�^
		virtual ~SmfController();	// �f�X�g���N�^
		
		//
		// ISmfController������
		//

		// SMF�I�u�W�F�N�g���擾
		Smf* GetSmf(){ return smf; }
		
		// SMF���g�p�\��Ԃɂ���
		virtual GGSERROR Open();
		
		// ����
		virtual void Close(){ smf->ReleaseCodeBinary(); }
		
		//

		// �t�@�C������ǂݍ���
		virtual GGSERROR LoadFromFile(LPCWSTR fileName);
		
		// ����������ǂݍ���
		virtual GGSERROR LoadFromMemory(const byte* smfBinary, int length);
		
		//

		// ������
		void Initialize(Analyzer::Analyzer* analyzerObject, Compiler::Compiler* compilerObject);
		
		//
		
	protected:
		// Analyzer�I�u�W�F�N�g
		Analyzer::Analyzer* GetAnalyzer(){ return analyzer; }
		
		// Compiler�I�u�W�F�N�g
		Compiler::Compiler* GetCompiler(){ return compiler; }
		
		// ��͌�ɌĂяo��(�ʏ�͉������Ȃ�)
		virtual void OnAfterAnalyzed(){}
		
	};

	/// <summary>
	/// SMF����N���X ver4�n���EBuffered�p
	/// </summary>
	class SmfController4Buffered : public SmfController
	{
	protected:
		// ���̎������R���p�C�����Ă��܂�
		void OnAfterAnalyzed(){ GetCompiler()->Compile(GetSmf()); }

	public:

		// SMF���g�p�\��Ԃɂ���(�������Ȃ�)
		GGSERROR Open(){ return GgsError::NoError; }
		
		// ����(�������Ȃ�)
		void Close(){}

	};
	
	/// <summary>
	/// SMF����N���X ver4�n���ENot Buffered�p
	/// </summary>
	class SmfController4NotBuffered : public SmfController
	{
	};
	
	/// <summary>
	/// SMF����N���X ver3�n���EBuffered�p
	/// </summary>
	class SmfController3Buffered : public SmfController
	{
	private:
		byte* smfBuffer;

	public:
		SmfController3Buffered();	// �R���X�g���N�^
		~SmfController3Buffered();	// �f�X�g���N�^

		// SMF���g�p�\��Ԃɂ���
		GGSERROR Open();

		// ����������ǂݍ���
		GGSERROR LoadFromMemory(const byte* smfBinary, int length);
	};
	
	/// <summary>
	/// SMF����N���X ver3�n���ENot Buffered�p
	/// </summary>
	class SmfController3NotBuffered : public SmfController
	{
	};
	


}}

#endif
