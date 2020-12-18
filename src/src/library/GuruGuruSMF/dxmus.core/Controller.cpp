#include "stdafx2.h"
#include "Controller.h"

namespace GuruGuruSmf{ namespace Dxmus{

Controller* Controller::instance = 0;

// �R���X�g���N�^
Controller::Controller()
{
	performancePlayer = new PerformancePlayer();
	port = new Port();
}

// �f�X�g���N�^
Controller::~Controller()
{
	delete performancePlayer;
	delete port;
}

// �C���X�^���X�擾
Controller* Controller::GetInstance()
{
	if(instance == 0){
		instance = new Controller();
	}
	return instance;
}

// �C���X�^���X�J��
void Controller::FreeInstance()
{
	delete instance;
	instance = 0;
}

}}
