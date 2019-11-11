#include "io_process.h"

std::map<size_t, std::unique_ptr<Process>> processes;
size_t first_free_process_ID = 0;

size_t Get_Free_Process_ID() {

	while (1) {
		first_free_process_ID++;
		if (processes.find(first_free_process_ID) == processes.end()) {
			return first_free_process_ID;
		}
	}
}

void Clone(kiv_hal::TRegisters &regs) {

	kiv_os::NClone clone_type = static_cast<kiv_os::NClone>(regs.rcx.r);

	switch (clone_type) {
		case kiv_os::NClone::Create_Process:
			Clone_Process(regs);
			break;
		case kiv_os::NClone::Create_Thread:
			Clone_Thread(regs);
			break;
	}

	//	Funkce procesu i vlakna maji prototyp TThread_Proc, protoze proces na zacatku bezi jako jedno vlakno,
	//		context.rdi v TThread_Proc pak pro proces ukazuji na retezec udavajiciho jeho argumenty, tj. co bylo dano do rdi
	//		a u vlakna je to pointer na jeho data
}

void Clone_Process(kiv_hal::TRegisters &regs) {
	// TODO Clone_Process: functional code.	

	char *export_name = reinterpret_cast<char*>(regs.rdx.r);
	char *arguments = reinterpret_cast<char*>(regs.rdi.r);
	char *working_directory;

	kiv_os::TThread_Proc entry_point = (kiv_os::TThread_Proc)GetProcAddress(User_Programs, export_name);

	//   |stdin|stdout| in hex
	//    |....|....| 
	//	  
	//	 stdin  = rbx.e >> 16
	//
	//					  |....|....|
	//   stdout = rbx.e & |0000|FFFF|
	//	
	kiv_os::THandle stdin_handle = regs.rbx.e >> 16;
	kiv_os::THandle stdout_handle = regs.rbx.e & 0x0000FFFF;

	std::unique_ptr<Process> process = std::make_unique<Process>(Get_Free_Process_ID(), working_directory);

	//	Create_Process: 
	//		bx obsahuje 2x THandle na stdin a stdout, tj. bx.e = (stdin << 16) | stdout
	//OUT - v programu, ktery zavolal Clone: ax je handle noveho procesu 
	//		ve spustenem programu:	ax a bx jsou hodnoty stdin a stdout, stderr pro jednoduchost nepodporujeme
}

void Clone_Thread(kiv_hal::TRegisters &regs) {
	// TODO Clone_Thread: functional code.

	//	Create_Thread a pak rdx je TThread_Proc a rdi jsou *data
	//OUT : rax je handle noveho procesu / threadu
}

void Wait_For(kiv_hal::TRegisters &regs) {
	// TODO Wait_For: functional code.

	//IN : rdx pointer na pole THandle, na ktere se ma cekat, rcx je pocet handlu
	//	funkce se vraci jakmile je signalizovan prvni handle
	//OUT : rax je index handle, ktery byl signalizovan
}

void Read_Exit_Code(kiv_hal::TRegisters &regs) {
	// TODO Read_Exit_Code: functional code.

	//IN:  dx je handle procesu/thread jehoz exit code se ma cist
	//OUT: cx je exitcode
}

void Exit(kiv_hal::TRegisters &regs) {
	// TODO Exit: functional code.

	//ukonci proces/vlakno
	//IN: cx je exit code
}

void Shutdown(kiv_hal::TRegisters &regs) {
	// TODO Shutdown: functional code.

	//nema parametry, nejprve korektne ukonci vsechny bezici procesy a pak kernel, cimz se preda rizeni do boot.exe, ktery provede simulaci vypnuti pocitace pres ACPI
}

void Register_Signal_Handler(kiv_hal::TRegisters &regs) {
	// TODO Register_Signal_Handler: functional code.

	//IN: rcx NSignal_Id, rdx 
	//	a) pointer na TThread_Proc, kde pri jeho volani context.rcx bude id signalu
	//	b) 0 a pak si OS dosadi defualtni obsluhu signalu
}


void Handle_Process(kiv_hal::TRegisters &regs) {

	switch (static_cast<kiv_os::NOS_Process>(regs.rax.l)) {

	case kiv_os::NOS_Process::Clone:
		Clone(regs);
		break;

	case kiv_os::NOS_Process::Wait_For:
		Wait_For(regs);
		break;

	case kiv_os::NOS_Process::Read_Exit_Code:
		Read_Exit_Code(regs);
		break;

	case kiv_os::NOS_Process::Exit:
		Exit(regs);
		break;

	case kiv_os::NOS_Process::Shutdown:
		Shutdown(regs);
		break;

	case kiv_os::NOS_Process::Register_Signal_Handler:
		Register_Signal_Handler(regs);
		break;
	}

}